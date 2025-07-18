#include <gl/glew.h>
#include "GL stuff.h"
#include "Mesh.h"
#include "scene 1.h"
#include <stdlib.h>
#include <time.h>
#include "vectors.h"
#include "physics.h"
#include "entityAdder.h"
#include "player.h"
#include "Debug.h"
#include "math.h"
#include "Dictionary.h"
#include "main.h"
#define MAX_ENEMY_COUNT 10U
#define ENEMY_TEXTURE_ID GL_TEXTURE1
#define ENEMY_TEXTURE_INDEX 1
#define SHADER_ENEMYTEXID 1.0f
#define SHADER_DISSOLVE_TEXID SHADER_ENEMYTEXID + .1f
#define TERRAIN_TEXTURE_ID GL_TEXTURE2
#define TERRAIN_TEXTURE_UNIT_INDEX 2
#define SHADER_TERRAIN_ID 2.0f
#define GROUND_COEF_RESTITUTION 10.0f
#define GROUND_FRICTION_COEF .0f
#define DEBUG_ENEMY_COLLIDER
#ifndef DEBUG_STUFF
#define DEBUG_STUFF
#define DEBUG_TEXTURE_ID GL_TEXTURE5
#define DEBUG_TEXTURE_INDEX 5
#define SHADER_DEBUG_TEX_ID 5.0f
#endif
#define ENEMY_HB_TEX_ID GL_TEXTURE6
#define ENEMY_HB_TEXTURE_INDEX 6
#define ENEMY_HB_SHADER_ID 6.0f
#define ENEMY_BORDER_TEX_ID GL_TEXTURE7
#define ENEMY_BORDER_TEXTURE_INDEX 7
#define ENEMY_BORDER_SHADER_ID 7.0f
#define MAX_DISSOLVE_ET 4.0f
#define SetElapsedTime(val) glUniform1f(GetLocVal2("u_elapsedT"), elapsedTime = val);
EnumWithMax(EnemyAnimState, idle, falling, flying_kick, hurt, axe_kick, kick, knee, punch, walk);
#define ENEMY_NUM_ANIMS maxEnemyAnimState + 1
Entity** enemies;
entityNode** enemyNodes;
int curEnemyIndex;
Transform* curTransform;
Vector2 oneVector = { 1.0f, 1.0f };
const Vector2 origEnemyScale = { -0.7225f, 0.52865852f };
const float healthBarYOffset = .23f;
uint noOfEnemies = MAX_ENEMY_COUNT;
static float enemyAccel = .3f;
/*
60: .31
144: .4
*/
static float enemySpeed = .04f;
/*
60: .017
144: .008
*/
float* blue[4];
bool* setMeshDissolve;
bool dissolved;
//1.0f is an arbitrary positive real number here
float elapsedTime = MAX_DISSOLVE_ET + 1.0f;
//to be called before SceneInit and TerrainInit
#ifdef DEBUG_ENEMY_COLLIDER
Mesh debugMesh[2];
Vector2 debugBoxSize;
#endif
Mesh debugMesh[2];
Vector2 debugBoxSize;
float* agro, minAgro;
Vector2 enemyAttkColOffsets[2] = { { -.12f, -.48f }, {.2f, .5f} };
Vector2 (*enemyAttkColPts)[NUM_VEC2_AABB];
Entity** opAttkEnts;//"opposition", to conserve the no. of chars. "ents" is short for "entities".
entityNode** opAttkNodes;
void SceneInit() {
	float blueColor[4] = { .0f, .2f, .7f, 1.f };
	int i;
	for (i = 0; i < lengthOf(blue); i++) {
		blue[i] = malloc(sizeof(float) * 4);
		for (byte j = 0; j < lengthOf(blueColor); j++) blue[i][j] = blueColor[j];
	}
	//enemyAccel = .1028f * logRefreshRate - .1109f;
	noOfEnemies = RandInt(1, MAX_ENEMY_COUNT);
	AllocPtrPtr(&opAttkEnts, noOfEnemies);
	AllocPtrPtr(&opAttkNodes, noOfEnemies);
	enemyAttkColPts = malloc(sizeof(Vector2[2]) * noOfEnemies);
	agro = calloc(noOfEnemies, sizeof(float));
	uintMap = calloc(noOfEnemies, sizeof(uintList*));
	enemyDamage = malloc(sizeof(float) * noOfEnemies);
	for (uint i = 0; i < noOfEnemies; i++) enemyDamage[i] = .1f;
	setMeshDissolve = calloc(noOfEnemies, sizeof(bool));
	monkTakenDmg = malloc(sizeof(bool) * noOfEnemies);
	printf("there are %u enemies\n", noOfEnemies);
	char *wrapper = "monk/sprites";
	//(in each animation)
	ushort noOfFrames[] = {
		4,
		2,
		2,
		2,
		5,
		7,
		2,
		6,
		6,
	};
	//the reason this is so disgustingly layered is because the most outer is the pointer to the heap alloc'ed vec***'s (so that a reference isn't stored in this script, it obviously only needs to be stored in the animator, the more inner pointer is then for every animation, then every animation frame, and then obviously every texture coordinate contains two vectors.
	Vector2**** animPts = malloc(sizeof(Vector2***));
	*animPts = malloc(sizeof(Vector2**) * ENEMY_NUM_ANIMS);
	ushort curNoFrames;
	for (uint i = 0; i < ENEMY_NUM_ANIMS; i++) {
		curNoFrames = noOfFrames[i];
		(*animPts)[i] = malloc(sizeof(Vector2*) * curNoFrames);
		for (uint j = 0; j < curNoFrames; j++) {
			(*animPts)[i][j] = malloc(sizeof(Vector2) * 2);
		}
	}
	float w = 1106.f, h = 180.f;
	DefaultAnimTexCoordPts(*animPts, .0f / w, 492.f / w, .0f / h, 51.f / h, walk, noOfFrames);
	DefaultAnimTexCoordPts(*animPts, 492.f / w, 820.f / w, .0f / h, 51.f / h, idle, noOfFrames);
	//63
	DefaultAnimTexCoordPts2(*animPts, 644.f / w, 1004.f / w, 61.f / h, 103.f / h, punch, noOfFrames, 63.f / w);
	Vector2* animScales = malloc(sizeof(Vector2) * EnemyAnimStateLength);
	bool iIsPunchTakeOne;
	for (uint i = 0; i < EnemyAnimStateLength; i += (iIsPunchTakeOne + 1)) {
		animScales[i] = oneVector;
		iIsPunchTakeOne = i == punch - 1;
	}
	animScales[punch].x = 63.f / 82.f;
	animScales[punch].y = 1.f;
	DefaultEntities_TexCoordAnim(enemies, enemyNodes, noOfEnemies, ENEMY_TEXTURE_ID, -.07f, -.465f, .03f, .355f, false, 1.0f, enemyLayer, SHADER_ENEMYTEXID, RandFloat(.0f, 15.0f), -.175f, origEnemyScale.x, origEnemyScale.y, enemyTag, "images/monk/player.png", ENEMY_TEXTURE_INDEX, wrapper, noOfFrames, AddUintVal2((uintptr_t)curEntity, i);, *animPts, animScales);
	enemyHBs = malloc(sizeof(HealthBar) * noOfEnemies);
	//different texture slot is used here (compared to player hb) because we're rendering in world space.
	CreateHealthBars(7.f, .001f, enemyHBs[i].barSlider, ENEMY_HB_SHADER_ID, ENEMY_HB_TEXTURE_INDEX, ENEMY_HB_TEX_ID, "images/Retro Inventory/Original/Health_03_Bar01.png", enemies[i]->physicsData->transform.position.x, enemies[i]->physicsData->transform.position.y + healthBarYOffset, .39f, .018f, noOfEnemies, ENEMY_BORDER_SHADER_ID, ENEMY_BORDER_TEXTURE_INDEX, ENEMY_BORDER_TEX_ID, "images/Retro Inventory/Original/Health_03.png", enemies[i]->physicsData->transform.position.x, enemies[i]->physicsData->transform.position.y + healthBarYOffset, 1.0f / 1.7f, .26f / 1.7f, noOfEnemies, enemyHBs[i].borderMesh, enemyHBs[i], monkHBNodes);
	debugBoxSize = VectorByScalar(oneVector, .03f);
#ifdef DEBUG_ENEMY_COLLIDER
	Vector2* positions = enemies[0]->origAABB;
	debugBoxSize = VectorByScalar(oneVector, .03f);
	float** colors[MESH_COLORS_LEN];
	uint j;
	for (i = 0; i < 2; i++) {
		colors[i] = malloc(sizeof(float*) * MESH_COLORS_LEN);
		for (j = 0; j < MESH_COLORS_LEN; j++) {
			colors[i][j] = calloc(MESH_NUM_COLORS, sizeof(float));
			colors[i][j][i] = colors[i][j][3] = 1.0f;
		}
	}
	//DefaultNewObjectsColor2(SHADER_DEBUG_TEX_ID, "images/debug/0.png", DEBUG_TEXTURE_INDEX, DEBUG_TEXTURE_ID, positions, debugBoxSize, colors, debugMesh, 2);
#endif
}
Entity** terrains;
entityNode** terrainNodes;
const uint noOfTerrains = 26;
void TerrainInit() {
	//TODO: turn this into a func.
	DefaultTerrain(terrains, terrainNodes, noOfTerrains, zeroVector, -.27f, -.467f, .27f, .366f, 1.0f, false, 1.0f, groundLayer, SHADER_TERRAIN_ID, (float)i * .5205f, -.829f, 1.0f, 1.0f, groundTag, TERRAIN_TEXTURE_UNIT_INDEX, "images/terrain/0.png", TERRAIN_TEXTURE_ID, .182243f, .2383178f, .2857143f, .4214286f);
	/*
	(*terrains)->physicsData->animator;
	(*terrains)->physicsData->bIsStatic;
	(*terrains)->physicsData->bIsTrigger;
	(*terrains)->physicsData->frictionCoef;
	(*terrains)->physicsData->gravityMultiplier;
	(*terrains)->physicsData->invMass;
	(*terrains)->physicsData->mass;
	(*terrains)->physicsData->mesh;
	(*terrains)->physicsData->restitutionCoef;
	(*terrains)->physicsData->solvedCol;
	(*terrains)->physicsData->transform.matchesScale;
	(*terrains)->physicsData->transform.pastPos;
	(*terrains)->physicsData->transform.position;
	(*terrains)->physicsData->transform.scale;
	(*terrains)->physicsData->transform.updateVerticies;
	(*terrains)->physicsData->velocity;*/
	/*for (uint i = 0; i < noOfTerrains; i++) {
		printf("%f, %f\n", Deconstruct(terrains[i]->physicsData->transform.position));
	}*/
}
float changeInPos;
float directionX;
float forceToAdd;
Vector2 healthBarPos;
PhysicsData* data;
HealthBar* curHB;
bool updateTime;
Entity* curEnemy;
bool resetTime;
bool currentAlive;
const float attkDst = .093f, walkDst = .2f;
bool isAgroed;
Animator* curAnim;
int agroAnim;
void SceneUpdate() {
	changeInPos = avgDT * enemyAccel;
	for (curEnemyIndex = 0; curEnemyIndex < noOfEnemies; curEnemyIndex++) {
		if (!(curEnemy = enemies[curEnemyIndex])) continue;
		data = curEnemy->physicsData;
		curAnim = data->animator;
		curTransform = &data->transform;
		healthBarPos = AddVectors(curTransform->position, VectorByScalar(upVector, healthBarYOffset));
		curHB = &enemyHBs[curEnemyIndex];
		MoveScaleMesh(curHB->borderMesh.meshLocation, healthBarPos, curHB->bordScale);
		MoveScaleSlider(curHB->barSlider.mesh, healthBarPos, curHB->barScale);
		currentAlive = curHB->barSlider.pseudoValue;
		if (!currentAlive) {
			if (dissolved) SetElapsedTime(.0f);
			enemyDamage[curEnemyIndex] = .0f;
			if (!setMeshDissolve[curEnemyIndex]) {
				ChangeTexInd2(data->mesh, SHADER_ENEMYTEXID + .1f);
				ChangeTexInd2(&curHB->borderMesh, ENEMY_BORDER_SHADER_ID + .1f);
				setMeshDissolve[curEnemyIndex] = true;
			}
			enemies[curEnemyIndex]->physicsData->animator->active = false;
			if (elapsedTime >= MAX_DISSOLVE_ET) {
				resetTime = true;
				DeleteQuad(data->mesh);
				DeleteQuad(&curHB->borderMesh);
				DeleteSlider(monkHBNodes[curEnemyIndex]);
				bool allNull = true;
				for (uint i = 0; i < noOfEnemies; i++) {
					if (curEnemyIndex != i && enemyNodes[curEnemyIndex]) {
						allNull = false;
						break;
					}
				}
				DeleteEntity(enemyNodes[curEnemyIndex], allNull);
				enemies[curEnemyIndex] = NULL;
				if (allNull) {
					free(enemyHBs);
				}
			}
			updateTime = true;
			continue;
		}
		if (SqrMagnitude(TakeVectors(playerData->transform.position, curTransform->position)) <= attkDst) {
			isAgroed = agro[curEnemyIndex] >= minAgro;
			agroAnim = idle * !isAgroed + punch * isAgroed;
			if (isAgroed && curAnim->animating != agroAnim) {
				ResetAnim(curAnim);
				SetColliderAABB(enemyAttkColPts[curEnemyIndex], enemyAttkColOffsets, data);
				opAttkEnts[curEnemyIndex] = (opAttkNodes[curEnemyIndex] = CreateCollider2(enemyAttkColPts[curEnemyIndex], enemyAttackTag, dontColLayer))->entity;
#ifdef DEBUG_ENEMY_COLLIDER
				for (uint i = 0; i < NUM_VEC2_AABB; i++) MoveScaleMesh(debugMesh[i].meshLocation, AddVectors(data->transform.position, VectorMult(data->transform.scale, enemyAttkColOffsets[i])), debugBoxSize);
#endif
			}
			curAnim->animating = agroAnim;
			continue;
		}
		curAnim->animating = walk;
		directionX = BoolToSign(playerData->transform.position.x < curTransform->position.x);
		forceToAdd = changeInPos * -directionX;
		//TODO: turn this into a function. goddamn
		AddForceScaleLimitVelX(*data, *curTransform, forceToAdd * (FAbs(data->velocity.x + forceToAdd) < enemySpeed), directionX * origEnemyScale.x, enemySpeed);
		//printf("moved entity %d by %f, player is %s the enemy\n", curEnemyIndex, FMin(FAbs(resultantVel), enemySpeed) * ((resultantVel > .0f) * 2.0f - 1.0f), playerData->transform.position.x < curTransform->position.x ? "to the left of" : "to the right of");
	}
	if (resetTime) {
		SetElapsedTime(.0f);
	}
	resetTime = false;
	if (!(dissolved = elapsedTime > MAX_DISSOLVE_ET) && updateTime) {
		glUniform1f(GetLocVal2("u_elapsedT"), elapsedTime += deltaTime());
	}
	updateTime = false;
}