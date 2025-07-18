#pragma once
#include <corecrt_malloc.h>
#include <Windows.h>
#include "Mesh.h"
#define SOLVE_ITTER 20
#define SOLVE_ITTER_F ((float)SOLVE_ITTER)
#define INV_SOLVE_ITTER_F 1.f / ((float)SOLVE_ITTER)
#define AABB_NUM_POINTS 2
#define ANIM_UPDATE_TIME .083333333f
#define GRID_COUNT_X 1024.0f
#define GRID_COUNT_Y 1024.0f
#define GRID_COUNT_X_INT 1024
#define GRID_COUNT_Y_INT 1024
#define linkedEntityList(head, next, prev) entityNode *head;\
entityNode* next;\
entityNode* prev
#define linkedListWValue(type, name, headName, _name)\
typedef struct _name name;\
struct _name {\
	type value;\
	name *next;\
	name *prev;\
};\
name* headName
#define linkedList(typeOfValue, nameOfList, _nameOfList)\
typedef struct _nameOfList nameOfList;\
struct _nameOfList {\
	typeOfValue *value;\
	nameOfList *next;\
	nameOfList *prev;\
}
#define linkedListWHead(type, name, headName, _name)\
typedef struct _name name;\
struct _name {\
	type *value;\
	name *next;\
	name *prev;\
};\
name* headName
#define checkGridSclCoord(add, coord){\
float size = (add->origAABB[1].coord - add->origAABB[0].coord) * 2.f;\
if (add->physicsData) size *= add->physicsData->transform.scale.coord;\
if (FAbs(size) > FAbs(cellSize.coord)) cellSize.coord = size;\
}
#define checkGridSclCoord2(add, coord){\
float size = (add->trueAABB[1].coord - add->trueAABB[0].coord) * 2.f;\
if (FAbs(size) > FAbs(cellSize.coord)) cellSize.coord = size;\
}
#define checkGridSclCoord3(add, coord){\
float size = (add[1].coord - add[0].coord) * 2.f;\
if (FAbs(size) > FAbs(cellSize.coord)) cellSize.coord = size;\
}
#define checkGridScale(add) checkGridSclCoord(add, x);\
checkGridSclCoord(add, y)
#define checkGridScale2(add) checkGridSclCoord2(add, x);\
checkGridSclCoord2(add, y)
#define checkGridScale3(add) checkGridSclCoord3(add, x);\
checkGridSclCoord3(add, y)
#define advance(node) node = node->next
#define NUM_THREADS 10
#define Insert(toAdd, head)\
toAdd->next = *head;\
toAdd->prev = NULL;\
if (*head) (*head)->prev = toAdd;\
*head = toAdd
#define Remove(remove, headPtr, type)\
type* previous = remove->prev, * next = remove->next;\
if (next) next->prev = previous;\
if (previous) {\
	previous->next = next;\
	goto ret;\
}\
*headPtr = next;\
ret:\
remove->next = remove->prev = NULL
#define Remove2(remove, head, type)\
type* previous = remove->prev, * next = remove->next;\
if (next) next->prev = previous;\
if (previous) {\
	previous->next = next;\
}\
else{\
head = next;\
}\
remove->next = NULL;\
remove->prev = NULL
//TODO: make below macro a func. then test if it works straight away.
#define updateAABB2(AABBName, entityName, unique_name_temp, data) AABBName = entityName->trueAABB;\
curAABB = entityName->origAABB;\
curColTransform = &data->transform;\
AABBName->x = curAABB->x * curColTransform->scale.x + curColTransform->position.x;\
AABBName->y = curAABB->y * curColTransform->scale.y + curColTransform->position.y;\
AABBName[1].x = curAABB[1].x * curColTransform->scale.x + curColTransform->position.x;\
AABBName[1].y = curAABB[1].y * curColTransform->scale.y + curColTransform->position.y;\
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
void UpdateEntities(void);
void UpdateSquishables(void);
void UpdateSliders(void);
void EntityInitialize();
typedef struct t_entityNode entityNode;
struct t_entityNode {
	Entity* entity;
	entityNode* next;
	entityNode* prev;
};
entityNode* entityHead;
Vector2 cellSize;
void InsertAtHead(Entity* add, Entity** head);
void RemoveEntityNode(entityNode* toRemove, entityNode **head);
bool cellCheckStates[GRID_COUNT_X_INT][GRID_COUNT_Y_INT];
typedef struct _cellChecks cellChecks;
struct _cellChecks {
	intVector2 value;
	cellChecks* next;
	cellChecks* prev;
};
cellChecks* checkedCells;
cellChecks *totalCheckedCells;
PTP_WORK cellCheckThreads[NUM_THREADS];
uint noOfCellChecks;
typedef enum _borderCell {
	currentCell = 0,
	right = 1,
	bottom_right = 2,
	bottom = 3,
	bottom_left = 4,
	left = 5,
	top_left = 6,
	top = 7,
	top_right = 8,
} borderCell;
typedef struct {
	entityNode* curNode;
	Vector2* curAABB, * curTrueAABB, * curSubTrueAABB;
	borderCell subBorderIndex;
	bool checkedSub;
	intVector2 checkColPtrVal;
	bool onRight, onLeft, onTop, onBottom;
	Entity* curCollider, * nextCollider, * subCollider;
	intVector2 curColPtrVal;
	uint index;
	uint limit;
	float dt;
} cellData;
cellData *cellDatas[NUM_THREADS];
//uint endOfCellChecks, cellCheckSection, cellCheckExcess;
uint nodeSection, endOfNodes, nodeExcess;
linkedList(SquishableMesh, squishList, _squishList);
linkedList(Slider, SliderList, _sliderList);
squishList *squishHead;
SliderList *sliderHead;
void RemoveEntity(Entity* toRemove, Entity** head);
Entity* cells[GRID_COUNT_X_INT][GRID_COUNT_Y_INT];
Entity* mainColHead;
Vector2* curAABB, * curTrueAABB, * curSubTrueAABB;
Transform* curColTransform;
inline void RemoveEntAppropriate(Entity* entity) {
	if (entity->pastCellPosition.x == -1) return;
	RemoveEntity(entity, (entity->pastCellPosition.x == GRID_COUNT_X_INT + 1 ? &mainColHead : &cells[entity->pastCellPosition.x][entity->pastCellPosition.y]));
}
inline void RemoveEntAppropriate2(Entity* entity, int position) {
	if (position == -1) return;
	RemoveEntity(entity, (entity->pastCellPosition.x == GRID_COUNT_X_INT + 1 ? &mainColHead : &cells[entity->pastCellPosition.x][entity->pastCellPosition.y]));
}
typedef uint(*UintDel)(ULONGLONG, uint);
//make sure "textureCoordPoints" is assigned on the heap. for this reason it should be a Vector2**** malloc'ed but declared locally.
#define DefaultEntities_TexCoordAnim(entities, entityNodes, noOfEntities, texture_ID, AABB_MinX, AABB_MinY, AABB_MaxX, AABB_MaxY, isTrigger, initialMass, initialLayer, shaderTextureID, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY, defaultTag, animatorPath, textureIndex, animatorWrapper, numberOfAnims, itterationDel, textureCoordPoints, animationScales)\
entityNodes = malloc(sizeof(entityNode*) * noOfEntities);\
entities = malloc(sizeof(Entity*) * noOfEntities);\
Entity* curEntity;\
PhysicsData* data;\
Animator* curAnimator;\
ushort numberOfAnimations = lengthOf(numberOfAnims);\
Vector2 pos = { defaultPosX, defaultPosY }, scale = { defaultScaleX, defaultScaleY };\
for (uint i = 0; i < noOfEntities; i++) {\
	data = ((curEntity = (entities[i] = malloc(sizeof(Entity))))->physicsData = malloc(sizeof(PhysicsData)));\
	curAnimator = (data->animator = malloc(sizeof(Animator)));\
	curAnimator->animScales = malloc(sizeof(Vector2) * numberOfAnimations);\
	for (uint j = 0; j < numberOfAnimations; j++){\
		curAnimator->animScales[j] = animationScales[j];\
	}\
	curAnimator->bUseTexCoordAnim = true;\
	curAnimator->noOfAnims = numberOfAnimations;\
	curAnimator->animFrameNo = malloc(sizeof(ushort) * numberOfAnimations);\
	curAnimator->texCoordPts = textureCoordPoints;\
	memcpy(curAnimator->animFrameNo, numberOfAnims, sizeof(*numberOfAnims) * numberOfAnimations);\
	curAnimator->animating = 0;\
	curAnimator->textureID = texture_ID;\
	curAnimator->animSpeed = 1.0f;\
	data->velocity = zeroVector;\
	data->gravityMultiplier = 1.0f;\
	curAnimator->active = true;\
	data->mesh = CreateQuad();\
	curEntity->origAABB->x = AABB_MinX;\
	curEntity->origAABB->y = AABB_MinY;\
	curEntity->origAABB[1].x = AABB_MaxX;\
	curEntity->origAABB[1].y = AABB_MaxY;\
	data->restitutionCoef = DEFAULT_RESTITUTION_COEF;\
	data->frictionCoef.x = DEFAULT_FRICTION_COEF_X;\
	data->frictionCoef.y = DEFAULT_FRICTION_COEF_Y;\
	data->bIsTrigger = isTrigger;\
	data->bIsStatic = false;\
	data->mass = initialMass;\
	itterationDel\
	curEntity->layer = initialLayer;\
	defaultMesh(data->mesh, shaderTextureID);\
	ChangePositionNScaleTo(data->transform, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY);\
	MoveScaleMesh(data->mesh->meshLocation, pos, scale);\
	curEntity->tag = defaultTag;\
	entityNodes[i] = SubscribeEntity_Without_Scale(curEntity);\
	curAnimator->animET = .0f;\
	curAnimator->curAnimFrame = -1;\
	InitSingleTexture(animatorPath, textureIndex, texture_ID, &curAnimator->frameTexture);\
}
#define DefaultEntities_TexCoordAnim2(entities, entityNodes, noOfEntities, texture_ID, AABB_MinX, AABB_MinY, AABB_MaxX, AABB_MaxY, isTrigger, initialMass, initialLayer, shaderTextureID, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY, defaultTag, animatorPath, textureIndex, animatorWrapper, numberOfAnims, itterationDel, textureCoordPoints, defaultAnim)\
entityNodes = malloc(sizeof(entityNode*) * noOfEntities);\
entities = malloc(sizeof(Entity*) * noOfEntities);\
Entity* curEntity;\
PhysicsData* data;\
Animator* curAnimator;\
ushort numberOfAnimations = lengthOf(numberOfAnims);\
Vector2 pos = { defaultPosX, defaultPosY }, scale = { defaultScaleX, defaultScaleY };\
for (uint i = 0; i < noOfEntities; i++) {\
	data = ((curEntity = (entities[i] = malloc(sizeof(Entity))))->physicsData = malloc(sizeof(PhysicsData)));\
	curAnimator = (data->animator = malloc(sizeof(Animator)));\
	curAnimator->bUseTexCoordAnim = true;\
	curAnimator->noOfAnims = numberOfAnimations;\
	curAnimator->animFrameNo = malloc(sizeof(ushort) * numberOfAnimations);\
	curAnimator->texCoordPts = textureCoordPoints;\
	memcpy(curAnimator->animFrameNo, numberOfAnims, sizeof(*numberOfAnims) * numberOfAnimations);\
	curAnimator->animating = defaultAnim;\
	curAnimator->textureID = texture_ID;\
	curAnimator->animSpeed = 1.0f;\
	data->velocity = zeroVector;\
	data->gravityMultiplier = 1.0f;\
	curAnimator->active = true;\
	data->mesh = CreateQuad();\
	curEntity->origAABB->x = AABB_MinX;\
	curEntity->origAABB->y = AABB_MinY;\
	curEntity->origAABB[1].x = AABB_MaxX;\
	curEntity->origAABB[1].y = AABB_MaxY;\
	data->restitutionCoef = DEFAULT_RESTITUTION_COEF;\
	data->frictionCoef.x = DEFAULT_FRICTION_COEF_X;\
	data->frictionCoef.y = DEFAULT_FRICTION_COEF_Y;\
	data->bIsTrigger = isTrigger;\
	data->bIsStatic = false;\
	data->mass = initialMass;\
	itterationDel\
	curEntity->layer = initialLayer;\
	defaultMesh(data->mesh, shaderTextureID);\
	ChangePositionNScaleTo(data->transform, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY);\
	MoveScaleMesh(data->mesh->meshLocation, pos, scale);\
	curEntity->tag = defaultTag;\
	entityNodes[i] = SubscribeEntity_Without_Scale(curEntity);\
	curAnimator->animET = .0f;\
	curAnimator->curAnimFrame = -1;\
	InitSingleTexture(animatorPath, textureIndex, texture_ID, &curAnimator->frameTexture);\
}