#pragma once
#include <stdbool.h>
#include <corecrt.h>
#include <GLFW/glfw3.h>
#include "vectors.h"
#include <Windows.h>
#include <stdio.h>
#define NUM_VEC2_AABB 2
#define DEFAULT_FRICTION_COEF_X .005f
#define DEFAULT_FRICTION_COEF_Y .005f
#define DEFAULT_RESTITUTION_COEF .0f
#define Min(a, b) a * (a < b) + b * (b < a)
#define Q_KEY 81U
#define K_KEY 75U
#define J_KEY 74U
#define R_KEY 0x52
#define SHADER_MAX_LINE_CHAR 100U//this is the maximum number of characters a shader can have *PER LINE*.
#define SHADER_MAX_CHAR 2500U//maximum number of characters shader will have *IN TOTAL*
#define D_KEY 68U
#define A_KEY 65U
#define W_KEY 87U
#define RIGHT_ARROW 39U
#define LEFT_ARROW 37U
#define UP_ARROW 38U
#define SPACE_KEY 32
#define lengthOf(arr) (sizeof(arr) / sizeof(arr[0]))
#define ChangeCameraPosition(positionX, positionY){\
camPosition.x = positionX;\
camPosition.y = positionY;\
shouldMoveCam = true;\
}
#define ChangeCameraPositionX(positionX)\
camPosition.x = positionX;\
shouldMoveCam = true
#define ChangeCameraPositionY(positionY){\
camPosition.y = positionY;\
shouldMoveCam = true;\
}
#define MAX_FPS_MULTIPLIER 3.0f
#define DefaultEntities(entities, entityNodes, noOfEntities, texture_ID, AABB_MinX, AABB_MinY, AABB_MaxX, AABB_MaxY, isTrigger, initialMass, initialLayer, shaderTextureID, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY, defaultTag, animatorPaths, textureIndex, animatorWrapper, numberOfAnims, itterationDel)\
entityNodes = malloc(sizeof(entityNode*) * noOfEntities);\
entities = malloc(sizeof(Entity*) * noOfEntities);\
Entity *curEntity;\
PhysicsData *data;\
Animator *curAnimator;\
ushort numberOfAnimations = lengthOf(numberOfAnims);\
Vector2 pos = { defaultPosX, defaultPosY }, scale = { defaultScaleX, defaultScaleY };\
for (uint i = 0; i < noOfEntities; i++){\
	data = ((curEntity = (entities[i] = malloc(sizeof(Entity))))->physicsData = malloc(sizeof(PhysicsData)));\
	curAnimator = (data->animator = malloc(sizeof(Animator)));\
	curAnimator->noOfAnims = numberOfAnimations;\
	curAnimator->animFrameNo = malloc(sizeof(ushort) * numberOfAnimations);\
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
	if (!i){\
		InitAnimator(data->animator, animatorPaths, textureIndex, animatorWrapper);\
		continue;\
	}\
	curAnimator->animET = .0f;\
	curAnimator->curAnimFrame = -1;\
	curAnimator->frameTextures = (*entities)->physicsData->animator->frameTextures;\
}
#define EnumWithMax(name, ...)\
typedef enum _##name{\
__VA_ARGS__,\
max##name = sizeof((int[]){__VA_ARGS__}) / sizeof(int) - 1,\
name##Length = max##name + 1,\
} name
#define DefaultEntities2(entities, entityNodes, noOfEntities, texture_ID, AABB_MinX, AABB_MinY, AABB_MaxX, AABB_MaxY, isTrigger, initialMass, initialLayer, shaderTextureID, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY, defaultTag, animatorPaths, textureIndex, animatorWrapper, numberOfAnims, itterationDel, initDel)\
entityNodes = malloc(sizeof(entityNode*) * noOfEntities);\
entities = malloc(sizeof(Entity*) * noOfEntities);\
Entity *curEntity;\
PhysicsData *data;\
Animator *curAnimator;\
ushort numberOfAnimations = lengthOf(numberOfAnims);\
Vector2 pos = { defaultPosX, defaultPosY }, scale = { defaultScaleX, defaultScaleY };\
for (uint i = 0; i < noOfEntities; i++){\
	data = ((curEntity = (entities[i] = malloc(sizeof(Entity))))->physicsData = malloc(sizeof(PhysicsData)));\
	curAnimator = (data->animator = malloc(sizeof(Animator)));\
	curAnimator->noOfAnims = numberOfAnimations;\
	curAnimator->animFrameNo = malloc(sizeof(ushort) * numberOfAnimations);\
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
	if (i){\
		curAnimator->animET = .0f;\
		curAnimator->curAnimFrame = -1;\
		curAnimator->frameTextures = (*entities)->physicsData->animator->frameTextures;\
	}\
	else{\
		initDel;\
		InitAnimator(data->animator, animatorPaths, textureIndex, animatorWrapper);\
	}\
	itterationDel\
	curEntity->layer = initialLayer;\
	defaultMesh(data->mesh, shaderTextureID);\
	ChangePositionNScaleTo(data->transform, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY);\
	MoveScaleMesh(data->mesh->meshLocation, pos, scale);\
	curEntity->tag = defaultTag;\
	entityNodes[i] = SubscribeEntity_Without_Scale(curEntity);\
}
#define DefaultStaticEntities(entities, entityNodes, noOfEntities, defaultVel, AABB_MinX, AABB_MinY, AABB_MaxX, AABB_MaxY, coefficient_of_restitution, frictionCoefficient, isTrigger, isStatic, initialMass, initialLayer, shaderTextureID, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY, defaultTag, textureIndex, texturePath, textureID)\
entityNodes = malloc(sizeof(entityNode*) * noOfEntities);\
entities = malloc(sizeof(Entity*) * noOfEntities);\
Entity *curEntity;\
for (uint i = 0; i < noOfEntities; i++){\
	entities[i] = malloc(sizeof(Entity));\
	curEntity = entities[i];\
	curEntity->animator = NULL;\
	curEntity->velocity = defaultVel;\
	curEntity->mesh = CreateQuad();\
	curEntity->origAABB->x = AABB_MinX;\
	curEntity->origAABB->y = AABB_MinY;\
	curEntity->origAABB[1].x = AABB_MaxX;\
	curEntity->origAABB[1].y = AABB_MaxY;\
	curEntity->restitutionCoef = coefficient_of_restitution;\
	curEntity->frictionCoef = frictionCoefficient;\
	curEntity->bIsTrigger = isTrigger;\
	curEntity->bIsStatic = isStatic;\
	curEntity->mass = initialMass;\
	curEntity->layer = initialLayer;\
	defaultMesh(curEntity->mesh, shaderTextureID);\
	ChangePositionNScaleTo(curEntity->transform, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY);\
	curEntity->tag = defaultTag;\
	entityNodes[i] = SubscribeEntity_Without_Scale(curEntity);\
	InitSingleTexture(texturePath, textureIndex, textureID);\
}
#define DefaultStaticEntitiesAtlas(entities, entityNodes, noOfEntities, defaultVel, AABB_MinX, AABB_MinY, AABB_MaxX, AABB_MaxY, coefficient_of_restitution, frictionCoefficientX, frictionCoefficientY, isTrigger, isStatic, initialMass, initialLayer, shaderTextureID, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY, defaultTag, textureIndex, texturePath, textureID, point1X, point1Y, point2X, point2Y)\
entityNodes = malloc(sizeof(entityNode*) * noOfEntities);\
entities = malloc(sizeof(Entity*) * noOfEntities);\
Entity *curEntity;\
for (uint i = 0; i < noOfEntities; i++){\
	entities[i] = malloc(sizeof(Entity));\
	curEntity = entities[i];\
	curEntity->animator = NULL;\
	curEntity->velocity = defaultVel;\
	curEntity->mesh = CreateQuad();\
	curEntity->origAABB->x = AABB_MinX;\
	curEntity->origAABB->y = AABB_MinY;\
	curEntity->origAABB[1].x = AABB_MaxX;\
	curEntity->origAABB[1].y = AABB_MaxY;\
	curEntity->restitutionCoef = coefficient_of_restitution;\
	curEntity->frictionCoef.x = frictionCoefficientX;\
	curEntity->frictionCoef.y = frictionCoefficientY;\
	curEntity->bIsTrigger = isTrigger;\
	curEntity->bIsStatic = isStatic;\
	curEntity->mass = initialMass;\
	curEntity->layer = initialLayer;\
	Vector2 points[2] = {\
	{point1X, point1Y},\
	{point2X, point2Y},\
	};\
	DefaultAtlasMesh(curEntity->mesh, shaderTextureID, points);\
	ChangePositionNScaleTo(curEntity->transform, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY);\
	curEntity->tag = defaultTag;\
	entityNodes[i] = SubscribeEntity_Without_Scale(curEntity);\
	InitSingleTexture2(texturePath, textureIndex, textureID);\
}
#define SetVec(vec, _x, _y) vec.x = _x; vec.y = _y
#define DefaultTerrain(entities, terrainNodes, noOfEntities, defaultVel, AABB_MinX, AABB_MinY, AABB_MaxX, AABB_MaxY, coefficient_of_restitution, isTrigger, initialMass, initialLayer, shaderTextureID, defaultPosX, defaultPosY, defaultScaleX, defaultScaleY, defaultTag, textureIndex, texturePath, textureID, point1X, point1Y, point2X, point2Y)\
entities = malloc(sizeof(Entity*) * noOfEntities);\
Entity *curEntity = NULL;\
PhysicsData *data;\
terrainNodes = malloc(sizeof(entityNode*) * noOfEntities);\
uint i;\
Vector2 scale = { defaultScaleX, defaultScaleY };\
Vector2 pos;\
pos.y = defaultPosY;\
for (i = 0; i < noOfEntities; i++){\
	data = ((curEntity = (entities[i] = malloc(sizeof(Entity))))->physicsData = calloc(1, sizeof(PhysicsData)));\
	terrainNodes[i] = malloc(sizeof(entityNode));\
	/*data->solvedCol = oneBVector;*/\
	data->animator = NULL;\
	data->velocity = defaultVel;\
	data->gravityMultiplier = .0f;\
	data->mesh = CreateQuad();\
	data->restitutionCoef = coefficient_of_restitution;\
	data->bIsTrigger = isTrigger;\
	data->bIsStatic = true;\
	data->mass = initialMass;\
	data->invMass = 1.0f / data->mass;\
	curEntity->layer = initialLayer;\
	data->transform.updateVerticies = zeroBVector;\
	data->transform.matchesScale = oneBVector;\
	Vector2 points[2] = {\
	{point1X, point1Y},\
	{point2X, point2Y},\
	};\
	DefaultAtlasMesh(data->mesh, shaderTextureID, points);\
	pos.x = defaultPosX;\
	curEntity->tag = defaultTag;\
	MoveScaleMesh(data->mesh->meshLocation, pos, scale);\
	InitSingleTexture2(texturePath, textureIndex, textureID);\
	ChangePositionNScaleTo(data->transform, pos.x, pos.y, defaultScaleX, defaultScaleY);\
	data->transform.pastPos = pos;\
	curEntity->origAABB->x = AABB_MinX;\
	curEntity->origAABB->y = AABB_MinY;\
	curEntity->origAABB[1].x = AABB_MaxX;\
	curEntity->origAABB[1].y = AABB_MaxY;\
	terrainNodes[i] = SubscribeEntity_Without_Scale(curEntity);\
}
//god mode.
//#define IS_DEV
#define NUM_KEYS 254
//only for single textures
#define DefaultNewObject(shaderTextureId, singleTexturePath, texture_index, texture_id, pos, size)\
Mesh mesh;\
CreateQuadAssign(&mesh);\
defaultMesh(&mesh, shaderTextureId);\
InitSingleTexture2(singleTexturePath, texture_index, texture_id);\
MoveScaleMesh(mesh.meshLocation, pos, size)
#define DefaultNewObjectColor(shaderTextureId, singleTexturePath, texture_index, texture_id, pos, size, color)\
Mesh newObjMesh;\
CreateQuadAssign(&newObjMesh);\
defaultMeshWithColor(&newObjMesh, shaderTextureId, color);\
InitSingleTexture2(singleTexturePath, texture_index, texture_id);\
MoveScaleMesh(newObjMesh.meshLocation, pos, size)
//non-local mesh
#define DefaultNewObjectColor2(shaderTextureId, singleTexturePath, texture_index, texture_id, pos, size, color, meshName)\
CreateQuadAssign(&meshName);\
defaultMeshWithColor(&meshName, shaderTextureId, color);\
InitSingleTexture2(singleTexturePath, texture_index, texture_id);\
MoveScaleMesh(meshName.meshLocation, pos, size)
#define DefaultNewObjectsColor2(shaderTextureId, singleTexturePath, texture_index, texture_id, positions, size, colors, meshName, noOfObjs)\
for (uint i = 0; i < noOfObjs; i++) {\
CreateQuadAssign(&meshName[i]);\
defaultMeshWithColor(&meshName[i], shaderTextureId, colors[i]);\
InitSingleTexture2(singleTexturePath, texture_index, texture_id);\
MoveScaleMesh(meshName[i].meshLocation, positions[i], size);\
}
#define Branch(True, False, condition) ((True) * (condition) + (False) * !(condition))
#define STRIDE 9
#define MESH_COLORS_LEN lengthOf(quadTemplate) / STRIDE
#define MESH_NUM_COLORS 4
#define PauseGame() timeScale = !timeScale
#define PressingN_Pressed(key) pressingKey[key] && pressedKey[key]
#define PressingN_NotPressed(key) pressingKey[key] && !pressedKey[key]
//local mesh. just the border.
#define CreateHealthBarBorder(shaderTexID, textureInd, textureId, path, posX, posY, scaleX, scaleY)\
Mesh hbMesh;\
CreateQuadAssign(&hbMesh);\
defaultMesh(&hbMesh, shaderTexID);\
InitSingleTexture2(path, textureInd, textureId);\
Vector2 hbPos = { posX, posY };\
Vector2 hbScale = { scaleX, scaleY };\
MoveScaleMesh(hbMesh.meshLocation, hbPos, hbScale)
//local meshes. just the borders.
#define CreateHealthBarBorders(shaderTexID, textureInd, textureId, path, posX, posY, scaleX, scaleY, numBorders, borderMesh, healthBar)\
for (uint i = 0; i < numBorders; i++){\
healthBar.bordPosition.x = posX;\
healthBar.bordPosition.y = posY;\
healthBar.bordScale.x = scaleX;\
healthBar.bordScale.y = scaleY;\
CreateQuadAssign(&borderMesh);\
defaultMesh(&borderMesh, shaderTexID);\
InitSingleTexture2(path, textureInd, textureId);\
MoveScaleMesh(borderMesh.meshLocation, healthBar.bordPosition, healthBar.bordScale);\
}
//just the inside bar.
#define CreateHealthBarBar(decreaseSpd, pseudoDist, slider, shaderTexId, textureInd, textureId, path, posX, posY, sclX, sclY)\
DefaultSMesh(slider, 1.0f, decreaseSpd, pseudoDist);\
CreateSQuadAssign(slider.mesh);\
defaultSMesh(slider.mesh, shaderTexId);\
InitSingleTexture2(path, textureInd, textureId);\
Vector2 hbPos = { posX, posY };\
Vector2 hbScl = { sclX, sclY };\
MoveScaleMesh(slider.mesh->meshLocation, hbPos, hbScl);\
SquishableMesh* mesh = slider.mesh;\
mesh->squishAmt = 1.0f;\
mesh->bSquish = false;\
hb_Node = SubscribeSlider(&slider)
//just the inside bars.
#define CreateHealthBarBars(decreaseSpd, pseudoDist, slider, shaderTexId, textureInd, textureId, path, posX, posY, sclX, sclY, numBars, healthBar, sliderNodes)\
sliderNodes = malloc(sizeof(SliderList*) * numBars);\
for (uint i = 0; i < numBars; i++){\
healthBar.barScale.x = sclX;\
healthBar.barScale.y = sclY;\
healthBar.barPosition.x = posX;\
healthBar.barPosition.y = posY;\
DefaultSMesh(slider, 1.0f, decreaseSpd, pseudoDist);\
CreateSQuadAssign(slider.mesh);\
defaultSMesh(slider.mesh, shaderTexId);\
InitSingleTexture2(path, textureInd, textureId);\
MoveScaleMesh(slider.mesh->meshLocation, healthBar.barPosition, healthBar.barScale);\
SquishableMesh* mesh = slider.mesh;\
mesh->squishAmt = 1.0f;\
mesh->bSquish = false;\
sliderNodes[i] = SubscribeSlider(&slider);\
}
//it is implied, but all args after bar args are border args
#define CreateHealthBars(decreaseSpd, pseudoDist, slider, barShaderTexId, barTextureInd, barTextureId, barPath, barPosX, barPosY, barSclX, barSclY, numBars, shaderTexID, textureInd, textureId, path, posX, posY, scaleX, scaleY, numBorders, borderMesh, healthBar, sliderNodes)\
CreateHealthBarBorders(shaderTexID, textureInd, textureId, path, posX, posY, scaleX, scaleY, numBorders, borderMesh, healthBar);\
CreateHealthBarBars(decreaseSpd, pseudoDist, slider, barShaderTexId, barTextureInd, barTextureId, barPath, barPosX, barPosY, barSclX, barSclY, numBars, healthBar, sliderNodes)
#define IsHoldKey1PressKey2(key1, key2) (PressingN_Pressed(key1) && PressingN_NotPressed(key2))
float frameTime;
typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ulong;
Vector2 camPosition;
LARGE_INTEGER pTime, cTime;
LONGLONG frequency;
float timeScale;
inline float deltaTime() {
	QueryPerformanceCounter(&cTime);
	return (float)((cTime.QuadPart - pTime.QuadPart) * 1000000 / frequency) / 1000000.0f * timeScale;
}
float avgDT;
inline float limitedDeltaTime() {
	return Min(deltaTime(), frameTime * MAX_FPS_MULTIPLIER);
}
inline float limitedDeltaTime2(float limitMultiplier) {
	return Min(deltaTime(), frameTime * limitMultiplier);
}
inline char* BoolToStr(bool b) {
	char* T = (char*)"true";
	char* F = (char*)"false";
	return (char*)((uintptr_t)T * b + (uintptr_t)F * !b);
}
bool pressingKey[NUM_KEYS];
bool pressedKey[NUM_KEYS];
inline bool pressing(int key);
bool shouldDraw;
int width, height, channels;
int camPosLoc;
bool shouldMoveCam;
float *vertices;
rsize_t verticesSize, indicesSize;
typedef struct ShaderProgramSrc {
	char* vertexSource;
	char* fragmentSource;
} ShaderProgramSource;
ShaderProgramSource ParseShader(char* filePath);
uint shader;
uint CreateShader(const char* vertexShader, const char* fragmentShader);
typedef long double decimal;
uint *indices;
int vSyncCount;
float refreshRate;
int strideOf2;
inline int RandInt(int min, int max) {
	return (int)(min + (max - min) * ((double)rand() / (double)RAND_MAX));
}
inline float RandFloat(float min, float max) {
	return min + (max - min) * (float)rand() / (float)RAND_MAX;
}
//if there is a ZeroMemory() in windows, it only makes sense to have a OneMemory().
inline void OneMemory(void* block, rsize_t size) {
	memset(block, 1, size);
}
inline void DefaultAnimTexCoordPts(Vector2*** pointsArr, float startPtX, float endPtX, float bottom, float top, int animIndex, ushort *noOfFrames) {
	ushort numFrames = noOfFrames[animIndex];
	float sectionLen = (endPtX - startPtX) / (float)numFrames;
	Vector2** points = pointsArr[animIndex];
	for (int i = 0; i < numFrames; i++) {
		points[i][0].y = bottom;
		points[i][1].y = top;
		points[i][0].x = startPtX + sectionLen * (float)i;
		points[i][1].x = startPtX + sectionLen * (float)(i + 1);
	}
}
inline void DefaultAnimTexCoordPts2(Vector2*** pointsArr, float startPtX, float endPtX, float bottom, float top, int animIndex, ushort *noOfFrames, float sectionLen) {
	ushort numFrames = noOfFrames[animIndex];
	Vector2** points = pointsArr[animIndex];
	for (int i = 0; i < numFrames; i++) {
		points[i][0].y = bottom;
		points[i][1].y = top;
		points[i][0].x = startPtX + sectionLen * (float)i;
		points[i][1].x = startPtX + sectionLen * (float)(i + 1);
	}
}
void AllocPtrPtr(void* dst, size_t );
void AllocNullStr(char** alloc, int strLen);