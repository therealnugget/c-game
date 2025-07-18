#include "EntityManager.h"//**IMPORTANT**: once an individual transform, rigidbody, AABB, or animator is needed, add another script just to be a that manager.
#include "math.h"
#include "Debug.h"
#include <stdio.h>
#include "player.h"
#define PROGRAM_IS_MULTITHREADED FALSE
#define GLOBAL_FRICTION_COEFFICIENT_X 17.f
#define GLOBAL_FRICTION_COEFFICIENT_Y .0f
#define updateAABB(AABBName, entityName, unique_name_temp) AABBName = entityName->trueAABB;\
data->curAABB = entityName->origAABB;\
curColTransform = &entityName->transform;\
AABBName->x = data->curAABB->x * curColTransform->scale.x + curColTransform->position.x;\
AABBName->y = data->curAABB->y * curColTransform->scale.y + curColTransform->position.y;\
AABBName[1].x = data->curAABB[1].x * curColTransform->scale.x + curColTransform->position.x;\
AABBName[1].y = data->curAABB[1].y * curColTransform->scale.y + curColTransform->position.y;\
float unique_name_temp;\
if (AABBName[1].x < AABBName->x){\
	unique_name_temp = AABBName->x;\
	AABBName->x = AABBName[1].x;\
	AABBName[1].x = unique_name_temp;\
}\
if (AABBName[1].y < AABBName->y){\
	unique_name_temp = AABBName->y;\
	AABBName->y = AABBName[1].y;\
	AABBName[1].y = unique_name_temp;\
}
#define SetCollider(col, link, ass) if (col->link->collider) col->link->collider = ass;\
else col->link->smallCollider = ass;
#define GetCurInd(prop) (curAnimator->prop[curAnimInd][curAnimator->curAnimFrame])
entityNode* curNode;
Entity* curMainColNode;
entityNode* entityHead = NULL;
Transform* curTransform;
Mesh* curMesh;
Animator* curAnimator;
float deltaT;
boolVector2 scale;
bool posIsZeroVec;
Vector2 zeroVector = { 0 };
boolVector2 zeroBVector = { 0 };
boolVector2 oneBVector = { true, true };
Entity* cells[GRID_COUNT_X_INT][GRID_COUNT_Y_INT] = { 0 };//a bunch of glorified linked list heads
bool cellCheckStates[GRID_COUNT_X_INT][GRID_COUNT_Y_INT];
TP_CALLBACK_ENVIRON callbackEnviron;
PTP_POOL pool;
PTP_CLEANUP_GROUP cleanupGroup;
PTP_WORK work;
PTP_WORK_CALLBACK workCallBack;
uint totalCheckCellIndex;
enum coord {
	y = 0,
	x = 1,
};
void RemoveEntityNode(entityNode* node, entityNode** head) {
	Remove(node, head, entityNode);
}
void RemoveEntity(Entity* toRemove, Entity** head) {
	Entity* previous = toRemove->prev, * next = toRemove->next;
	if (next) next->prev = previous;
	if (previous)
	{
		previous->next = next;
		goto ret;
	}
	*head = next;
ret:
	toRemove->next = toRemove->prev = NULL;
}
void InsertAtHead(Entity* add, Entity** head) {
	add->next = *head;
	add->prev = NULL;
	if (*head) (*head)->prev = add;
	*head = add;
}
void InsertAtHeadCellChecks(cellChecks* this, intVector2 add) {
	this->value = add;
	this->next = checkedCells;
	this->prev = NULL;
	if (checkedCells) (checkedCells)->prev = this;
	checkedCells = this;
}
void RemoveCellChecks(cellChecks* toRemove) {
	cellChecks* previous = toRemove->prev, * next = toRemove->next;
	if (next) next->prev = previous;
	if (previous)
	{
		previous->next = next;
		goto ret;
	}
	checkedCells = next;
ret:
	toRemove->next = toRemove->prev = NULL;
}
void SortGridCell(Entity* entity) {
	PhysicsData* data = entity->physicsData;
	intVector2 pastCellPos = entity->pastCellPosition;
	intVector2 cellPositionInGrid = { (int)roundf(entity->midPoint.x / cellSize.x + GRID_COUNT_X / 2.0f), (int)roundf(entity->midPoint.y / cellSize.y + GRID_COUNT_Y / 2.0f) };
	if (!memcmp(&cellPositionInGrid, &pastCellPos, sizeof(Vector2))) return;
	RemoveEntAppropriate2(entity, pastCellPos.x);
	if (cellPositionInGrid.x > 0 && cellPositionInGrid.y > 0 && cellPositionInGrid.x < GRID_COUNT_X_INT && cellPositionInGrid.y < GRID_COUNT_Y_INT) {
		Entity* currentCell = cells[cellPositionInGrid.x][cellPositionInGrid.y];
		entity->next = currentCell;
		if (currentCell) currentCell->prev = entity;
		cells[cellPositionInGrid.x][cellPositionInGrid.y] = entity;
		entity->pastCellPosition = cellPositionInGrid;
//#define SORT_GRID_CELL_DEBUG
#ifdef SORT_GRID_CELL_DEBUG
		char* name = NULL;
		switch (entity->tag) {
			#define SwitchName(tag, nameToAssign)\
		case tag:\
			name = nameToAssign;\
			break
			SwitchName(enemyTag, "enemy");
			SwitchName(playerTag, "player");
			SwitchName(groundTag, "ground");
			SwitchName(playerAttackTag, "player attack");
		}
		printf("%s with MP of %f, %f was sorted into (%d, %d)\n", name, Deconstruct(entity->midPoint), cellPositionInGrid.x, cellPositionInGrid.y);
#endif
		/*
		printf("%s", "data with name ");
		switch (data->tag) {
#define SwitchCase(_case, behav) case _case: behav; break;
#define PrintF(str) printf("%s", str);
			SwitchCase(enemyTag, PrintF("enemy"));
			SwitchCase(groundTag, PrintF("platform"));
			SwitchCase(playerTag, PrintF("player"));
		}
		printf(" was sorted into cell (%d, %d.)\n", cellPositionInGrid.x, cellPositionInGrid.y);*/
	}
	else {
		entity->next = mainColHead;
		if (mainColHead) mainColHead->prev = entity;
		mainColHead = entity;
		entity->pastCellPosition.x = GRID_COUNT_X_INT + 1;
	}
	entity->prev = NULL;
}
#if PROGRAM_IS_MULTITHREADED
HANDLE hMutex, collisionMutex;
VOID CALLBACK CheckCell();
#endif
bool cellCheckStates[GRID_COUNT_X_INT][GRID_COUNT_Y_INT] = { 0 };
int curAnimInd;
void EntityInitialize() {//this only needs to be called any time before the first main update
	mainColHead = NULL;
#if PROGRAM_IS_MULTITHREADED
	uint i;
	workCallBack = CheckCell;
	hMutex = CreateMutex(NULL, FALSE, NULL);
	collisionMutex = CreateMutex(NULL, FALSE, NULL);
	for (i = 0; i < lengthOf(cellDatas); i++) cellDatas[i] = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(cellData));
	InitializeThreadpoolEnvironment(&callbackEnviron);
	pool = CreateThreadpool(NULL);
	SetThreadpoolThreadMinimum(pool, 0);
	SetThreadpoolThreadMaximum(pool, NUM_THREADS);
	cleanupGroup = CreateThreadpoolCleanupGroup();
	SetThreadpoolCallbackPool(&callbackEnviron, pool);
	SetThreadpoolCallbackCleanupGroup(&callbackEnviron, cleanupGroup, NULL);
#endif
}
cellChecks* curCheckedCell, * prevCheckedCell;
uint curThread, checkCellIndex;
#if !FAST_SOLVE
uint solveIndex;
#endif
entityNode* curNode;
borderCell subBorderIndex;
bool checkedSub;
intVector2 checkColPtrVal;
bool onRight, onLeft, onTop, onBottom;
Entity* curCollider, * subCollider;
intVector2 curColPtrVal;
uint index;
Entity* curEntity;
PhysicsData* curData, * subData;
void UpdateEntities(void) {
	plrGrounded = false;//removed mt code because it made searching annoying. it's bound to be in a previous version on one of my pcs somewhere but if it gets lost don't worry, it's absolute trash anyway. the code i did for the much newer multithreaded quadtree collision solver i wrote in c++ is much better and actually works.
	for (solveIndex = 0; (solveIndex < SOLVE_ITTER); solveIndex++) {
		curNode = entityHead;
		while (curNode) {
			if (!(curData = curNode->entity->physicsData)) goto preColSolvCont;
#ifdef IS_DEV
			AddForceY(*curData, curData->transform, gravAccel * limitedDeltaTime2(2.0f) * !curData->bIsStatic * curData->gravityMultiplier * (curNode->entity->tag != playerTag || pressingKey[K_KEY] || pressingKey[J_KEY]) * (pressingKey[K_KEY] - pressingKey[J_KEY])) / SOLVE_ITTER_F;
#else
			AddForceY(*curData, curData->transform, gravAccel * limitedDeltaTime2(2.0f) * !curData->bIsStatic * curData->gravityMultiplier / SOLVE_ITTER_F);
#endif
			AddVectorsAssign(&curData->transform.position, VectorByScalar(curData->velocity, INV_SOLVE_ITTER_F * deltaTime()));
		preColSolvCont:
			advance(curNode);
		}
		curNode = entityHead;
		while (curNode) {
			curData = (curEntity = curNode->entity)->physicsData;
			if (curData) {
				curEntity->midPoint = AddVectors(curEntity->origMidpoint, curData->transform.position);
			}
			else {
				curEntity->midPoint = GetVectorMidPoint(curEntity->trueAABB);
			}
			SortGridCell(curEntity);
			curEntity->bMadeTrueAABB = false;
			advance(curNode);
		}
		index = 0;
		curNode = entityHead;
		while (curNode) {
			curData = (curEntity = curNode->entity)->physicsData;
			curColPtrVal = curEntity->pastCellPosition;
			if (curColPtrVal.x != -1 && curColPtrVal.x != GRID_COUNT_X_INT + 1) curCollider = cells[curColPtrVal.x][curColPtrVal.y];
			else goto cont;
			if (cellCheckStates[curColPtrVal.x][curColPtrVal.y]) goto cont;
			cellCheckStates[curColPtrVal.x][curColPtrVal.y] = true;
			InsertAtHeadCellChecks(&totalCheckedCells[index++], curColPtrVal);
			onLeft = curColPtrVal.x == 0;
			onBottom = curColPtrVal.y == 0;
			onRight = curColPtrVal.x == GRID_COUNT_X_INT - 1;
			onTop = curColPtrVal.y == GRID_COUNT_Y_INT - 1;
			while (curCollider) {
				subBorderIndex = currentCell;
				subCollider = curCollider->next;
				curData = curCollider->physicsData;
				if (!subCollider) goto _switch;
			iterate:
				checkedSub = false;
				while (subCollider) {
					subData = subCollider->physicsData;
					if (curData && !checkedSub && !curCollider->bMadeTrueAABB) {
						updateAABB2(curTrueAABB, curCollider, temp, curData);
						curCollider->bMadeTrueAABB = true;
					}
					checkedSub = true;
					if (subData && !subCollider->bMadeTrueAABB) {
						updateAABB2(curSubTrueAABB, subCollider, temp2, subData);
						subCollider->bMadeTrueAABB = true;
					}
					if (curCollider != subCollider) AABBsOverlap(curCollider, subCollider);
					advance(subCollider);
				}
			_switch:
				subBorderIndex++;
			afterInc:
				if (subBorderIndex >= 9) {
					advance(curCollider);
					continue;
				}
				switch (subBorderIndex) {
				case right:
					if (onRight) goto _switch;
					checkColPtrVal.x = curColPtrVal.x + 1;
					checkColPtrVal.y = curColPtrVal.y;
					break;
				case bottom_right:
					if (onBottom || onRight) goto _switch;
					checkColPtrVal.x = curColPtrVal.x + 1;
					checkColPtrVal.y = curColPtrVal.y - 1;
					break;
				case bottom:
					if (onBottom) goto _switch;
					checkColPtrVal.x = curColPtrVal.x;
					checkColPtrVal.y = curColPtrVal.y - 1;
					break;
				case bottom_left:
					if (onBottom || onLeft) goto _switch;
					checkColPtrVal.x = curColPtrVal.x - 1;
					checkColPtrVal.y = curColPtrVal.y - 1;
					break;
				case left:
					if (onLeft) goto _switch;
					checkColPtrVal.x = curColPtrVal.x - 1;
					checkColPtrVal.y = curColPtrVal.y;
					break;
				case top_left:
					if (onTop || onLeft) goto _switch;
					checkColPtrVal.x = curColPtrVal.x - 1;
					checkColPtrVal.y = curColPtrVal.y + 1;
					break;
				case top:
					if (onTop) goto _switch;
					checkColPtrVal.x = curColPtrVal.x;
					checkColPtrVal.y = curColPtrVal.y + 1;
					break;
				case top_right:
					if (onTop || onRight) goto _switch;
					checkColPtrVal.x = curColPtrVal.x + 1;
					checkColPtrVal.y = curColPtrVal.y + 1;
					break;
				}
				subCollider = cells[checkColPtrVal.x][checkColPtrVal.y];
				if (cellCheckStates[checkColPtrVal.x][checkColPtrVal.y]) goto _switch;
				goto iterate;
			}
		cont:
			advance(curNode);
		}
		curCheckedCell = checkedCells;
		while (curCheckedCell) {
			cellCheckStates[curCheckedCell->value.x][curCheckedCell->value.y] = false;
			prevCheckedCell = curCheckedCell;
			advance(curCheckedCell);
			RemoveCellChecks(prevCheckedCell);
		}
		checkedCells = NULL;
	}
	curNode = entityHead;
	deltaT = deltaTime();
	while (curNode) {
		curData = curNode->entity->physicsData;
		if (!curData) goto transformCont;
		curEntity = curNode->entity;
		curTransform = &curData->transform;
#define ApplyFriction(coord, globalCoef) curData->velocity.coord -= (curData->velocity.coord < .0f ? FMax : FMin)(curData->frictionCoef.coord * globalCoef * ((curData->velocity.coord > .0f) * 2.0f - 1.0f) * curData->invMass * (bool)curData->velocity.coord, curData->velocity.coord);
#define XOR_UpdateVert(coord) curTransform->updateVerticies.coord |= !curData->bIsStatic && memcmp(&curTransform->pastPos, &curTransform->position, vecSize);\
shouldDraw |= curTransform->updateVerticies.coord
		ApplyFriction(x, GLOBAL_FRICTION_COEFFICIENT_X);
		ApplyFriction(y, GLOBAL_FRICTION_COEFFICIENT_Y);
		XOR_UpdateVert(x);
		XOR_UpdateVert(y);
		curMesh = curData->mesh;
		curAnimator = curData->animator;
		posIsZeroVec = !memcmp(&(curTransform->position), &zeroVector, sizeof(Vector2));
		scale.x = curTransform->scale.x != 1.0f || !curTransform->matchesScale.x;
		scale.y = curTransform->scale.y != 1.0f || !curTransform->matchesScale.y;
		if (curTransform->updateVerticies.x && curTransform->updateVerticies.y) {
			ResetScaleUpdate(curTransform);
			ResetVertexUpdate(curTransform);
			if (posIsZeroVec) {
				ScaleMesh(curMesh->meshLocation, curTransform->scale.x, curTransform->scale.y);
			}
			else {
				if (scale.x || scale.y) {
					MoveScaleMesh(curMesh->meshLocation, curTransform->position, curTransform->scale);
				}
				else MoveMeshTo(curMesh->meshLocation, curTransform->position);
			}
		}
		else {
			if (curTransform->updateVerticies.x) {//we set the mesh to a position on the x and y instead of ADDING BY a position on the x and y, in case the position was changed twice by two different factors in one frame.
				curTransform->updateVerticies.x = false;
				if (!scale.x) MoveMeshX(curMesh->meshLocation, curTransform->position.x);
				else goto changeScaleX;
			}
			if (scale.x) {
			changeScaleX:
				checkGridSclCoord(curNode->entity, x);
				curTransform->matchesScale.x = true;
				if (posIsZeroVec) {
					ScaleMeshX(curMesh->meshLocation, curTransform->scale.x);
				}
				else {
					MoveScaleMeshX(curMesh->meshLocation, curTransform->position.x, curTransform->scale.x);
				}
			}
			if (curTransform->updateVerticies.y) {
				curTransform->updateVerticies.y = false;
				if (!scale.y) MoveMeshY(curMesh->meshLocation, curTransform->position.y);
				else goto changeScaleY;
			}
			if (scale.y) {
			changeScaleY:
				checkGridSclCoord(curNode->entity, y);
				curTransform->matchesScale.y = true;
				if (posIsZeroVec) ScaleMeshY(curMesh->meshLocation, curTransform->scale.y);
				else MoveScaleMeshY(curMesh->meshLocation, curTransform->position.y, curTransform->scale.y);
			}
		}
		if (curAnimator) {
			if (curAnimator->animET >= ANIM_UPDATE_TIME / curAnimator->animSpeed) {
				if (curAnimator->active) {
					curAnimator->animET = .0f;
					curAnimator->curAnimFrame = (ushort)((ushort)(curAnimator->curAnimFrame + 1) % curAnimator->animFrameNo[curAnimator->animating]);
					glActiveTexture(curAnimator->textureID);
#define BindTex(tex) glBindTexture(GL_TEXTURE_2D, tex);
					curAnimInd = curAnimator->animating;
					if (curAnimator->bUseTexCoordAnim) {
						ChangeTexCoordTo(curMesh, GetCurInd(texCoordPts));
						BindTex(curAnimator->frameTexture);
						goto useTexCoordAnim;
					}
					else {
						BindTex(GetCurInd(frameTextures));
					}
				}
			}
			else curAnimator->animET += deltaT;
			if (curAnimator->bUseTexCoordAnim) {
				curAnimInd = curAnimator->animating;
			useTexCoordAnim:
				if (curAnimator->curAnimFrame >= 0 && curAnimator->animScales) {
					MoveScaleMesh(curMesh->meshLocation, curTransform->position, VectorMult(curAnimator->animScales[curAnimInd], curTransform->scale));
				}
			}
		}
		curTransform->pastPos = curTransform->position;
	transformCont:
		advance(curNode);
	}
}
squishList* curSquishable;
SquishableMesh* curSquishMesh;
void UpdateSquishables(void) {
	curSquishable = squishHead;
	while (curSquishable) {
		curSquishMesh = curSquishable->value;
		if (!curSquishMesh->bSquish) goto cont;
		SquishMeshX(curSquishMesh);
		curSquishMesh->bSquish = false;
	cont:
		advance(curSquishable);
	}
}
SliderList* curSliderNode;
Slider* curSlider;
void UpdateSliders(void) {
	curSliderNode = sliderHead;
	while (curSliderNode) {
		curSlider = curSliderNode->value;
		curSquishMesh = curSlider->mesh;
		//printf("%s, %f, %f\n", BoolToStr(curSquishMesh->bSquish), curSquishMesh->squishAmt, curSlider->pseudoValue);
		if (!curSquishMesh->bSquish) goto cont;
		if (FAbs(curSquishMesh->squishAmt - curSlider->pseudoValue) <= curSlider->pseudoDist) {
			curSquishMesh->bSquish = false;
			SquishMeshX2(curSquishMesh, curSlider->pseudoValue);
		}
		SquishMeshX2(curSquishMesh, Lerp(curSquishMesh->squishAmt, curSlider->pseudoValue, deltaTime() * curSlider->decreaseSpd));
	cont:
		advance(curSliderNode);
	}
}