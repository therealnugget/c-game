#pragma once
#include "animations.h"
#include "Physics.h"
//do NOT use these transform properties directly. instead, use the definitions.
#define ChangeScaleXTo(transform, scaleX)\
(transform).scale.x = scaleX;\
(transform).matchesScale.x = false;\
shouldDraw = true
#define ChangeScaleTo(transform, scaleX, scaleY)\
(transform).scale.x = scaleX;\
(transform).scale.y = scaleY;\
(transform).matchesScale = zeroBVector;\
shouldDraw = true
#define ChangeScaleYTo(transform, scaleY){\
(transform).scale.y = scaleY;\
(transform).matchesScale.y = false;\
shouldDraw = true;\
}
#define ChangePositionXTo(transform, positionX)\
(transform).position.x = positionX;\
(transform).updateVerticies.x = true;\
shouldDraw = true
#define ChangePositionNScaleXTo(transform, positionX, scaleX)\
(transform).position.x = positionX;\
(transform).scale.x = scaleX;\
(transform).updateVerticies.x = true;\
(transform).matchesScale.x = false;\
shouldDraw = true
#define AddForceScaleLimitVelX(entity, transform, force, _scale, velocityLimit)\
float resultantVel = (entity).velocity.x + force;\
(entity).velocity.x = FMin(FAbs(resultantVel), velocityLimit) * ((resultantVel > .0f) * 2.0f - 1.0f);\
(transform).scale.x = _scale;\
(transform).matchesScale.x = false;\
(transform).updateVerticies.x = true;\
shouldDraw = true
#define AddForceLimitVelX(entity, transform, force, velocityLimit)\
float resultantVel = (entity).velocity.x + force;\
(entity).velocity.x = FMin(FAbs(resultantVel), velocityLimit) * ((resultantVel > .0f) * 2.0f - 1.0f);\
(transform).updateVerticies.x = true;\
shouldDraw = true
#define AddForceX(entity, force)\
(entity).velocity.x += force;\
(entity).transform.updateVerticies.x = true;\
shouldDraw = true
#define AddForceY(entity, transform, force)\
(entity).velocity.y += force;\
(transform).updateVerticies.y = true;\
shouldDraw = true
#define ChangePositionYTo(transform, positionY)\
(transform).position.y = positionY;\
(transform).updateVerticies.y = true;\
shouldDraw = true
#define ChangePositionNScaleYTo(transform, positionY, scaleY){\
(transform).position.y = positionY;\
(transform).scale.y = scaleY;\
(transform).updateVerticies.y = true;\
(transform).matchesScale.y = false;\
shouldDraw = true;\
}
#define ChangePositionNScaleTo(transform, positionX, positionY, scaleX, scaleY)\
(transform).position.x = positionX;\
(transform).position.y = positionY;\
(transform).scale.x = scaleX;\
(transform).scale.y = scaleY;\
(transform).updateVerticies = oneBVector;\
(transform).matchesScale = zeroBVector;\
shouldDraw = true
#define SquishX(mesh, amount)\
(mesh).squishAmt -= FMin(amount, (mesh).squishAmt);\
(mesh).bSquish = true;
#define TakeSliderVal(slider, amount)\
(slider).pseudoValue -= FMin(amount, (slider).pseudoValue);\
(slider).mesh->bSquish = true
#define MoveScaleSlider(mesh, pos, scale)\
MoveScaleMesh((mesh)->meshLocation, pos, scale);\
(mesh)->origXMinY = vertices[mesh->meshLocation + stride];\
(mesh)->origXMaxY = vertices[mesh->meshLocation + stride * 2];\
(mesh)->bSquish = true
#define DefaultSMesh(slider, pseudoVal, decreaseSpeed, pseudoDistance)\
(slider).mesh = malloc(sizeof(SquishableMesh));\
(slider).pseudoValue = pseudoVal;\
(slider).pseudoDist = pseudoDistance;\
(slider).decreaseSpd = decreaseSpeed
//for colliders which aren't physics objects (i.e. they don't have physics-based collisions, or trigger colliders).
//#define SetColliderAABB(col, scaleX, scaleY, offsets, data)\
//for (int i = 0; i < lengthOf(col); i++) {\
//	(col)[i].x = data->transform.position.x + offsets[i].x * scaleX;\
//	(col)[i].y = data->transform.position.y + offsets[i].y * scaleY;\
//}\
//checkGridScale3(col)
typedef struct _Mesh {
	unsigned int meshLocation;
	unsigned int indiciesLoc;
} Mesh;
typedef struct _SquishableMesh {
	unsigned int meshLocation;
	unsigned int indiciesLoc;
	float origXMaxY, origXMinY;
	float squishAmt;
	bool bSquish;
} SquishableMesh;
typedef struct _Slider {
	SquishableMesh *mesh;
	float pseudoValue;
	float decreaseSpd;
	float pseudoDist;//the distance the actual value has to be from the pseudo value before the slider value can stop lerping
} Slider;
uint posTempLen, indTempLen, indTempSize, quadTempLen, quadTempSize;//tempLATES length, NOT tempORARY.
unsigned int *delMesh, *delInd;
uint stride;
uint strideTakeTwo;
rsize_t delMeshSize, delIndSize;
//Mesh *CreateMesh(ushort noOfQuads);
Mesh *CreateQuad();
void CreateQuadAssign(Mesh *mesh);
void CreateSQuadAssign(SquishableMesh* mesh);
void defaultMesh(Mesh* mesh, float textureUnitIndex);
void defaultSMesh(SquishableMesh* mesh, float textureUnitIndex);
void DefaultMeshNotPos(Mesh* mesh, float textureUnitIndex);
void defaultMeshWithColor(Mesh* mesh, float textureUnitIndex, float* colors[]);
void ChangeMeshColor(uint meshLoc, float* colors[]);
void DefaultAtlasMesh(Mesh* mesh, float textureUnitIndex, Vector2 points[2]);//points must be normalized before their passing in; x's then y's for points.
void MoveMeshX(unsigned int meshLoc, float toMoveTo);
void MoveMeshY(unsigned int meshLoc, float toMoveTo);
void MoveScaleMesh(unsigned int meshLoc, Vector2 position, Vector2 scale);
void MoveMeshTo(unsigned int meshLoc, Vector2 position);
void MoveScaleMeshX(unsigned int meshLoc, float toMoveTo, float scale);
void MoveScaleMeshY(unsigned int meshLoc, float toMoveTo, float scale);
void ScaleMesh(unsigned int meshLoc, float scaleX, float scaleY);
void ScaleMeshX(unsigned int meshLoc, float scaleX);
void ScaleMeshY(unsigned int meshLoc, float scaleY);
void SquishMeshX2(SquishableMesh* mesh, float amount);
void SquishMeshX(SquishableMesh* mesh);
void SquishSliderhX(SquishableMesh* mesh);
void DeleteQuad(Mesh *mesh);
void DeleteSQuad(SquishableMesh* mesh);
void ChangeTexInd(Mesh* mesh, float textureUnitIndex[4]);
void ChangeTexInd2(Mesh* mesh, float);
void ChangeTexCoordTo(Mesh* mesh, Vector2* newVertices);
inline void BufferIndicies() {
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_DYNAMIC_DRAW);
}
void MeshInitialize();
void MeshStartup();
float positionTemplate[8];
float texcoordTemplate[8];
float colorTemplate[16];
float quadTemplate[36];
unsigned int delMeshLen, delIndLen;
unsigned int curMeshLoc;
unsigned int curIndiciesLoc;
unsigned int indiciesTemplate[6];
unsigned int vertexIterator;
int textureIDs[32];
int GetOrigVertex(uint vertex);
typedef struct _Transform {
	boolVector2 updateVerticies, matchesScale;
	Vector2 scale;
	Vector2 position;
	Vector2 pastPos;
} Transform;
typedef enum {
	defaultTag,
	playerTag,
	enemyTag,
	groundTag,
	playerAttackTag,
	enemyAttackTag,
} tag;
typedef struct _PhysicsData PhysicsData;
typedef struct _Entity Entity;
struct _Entity {//physics objects, which have animators, meshes, and colliders.
	struct _PhysicsData {
		Transform transform;
		Mesh* mesh;
		Animator* animator;
		float gravityMultiplier;
		float restitutionCoef;//coefficient of restitution
		Vector2 frictionCoef;
		float mass, invMass;
		Vector2 velocity;
		bool bIsStatic;
		bool bIsTrigger;
	} *physicsData;
	enum _layerName layer;
	intVector2 pastCellPosition;//one positively outside of grid bounds in the x-axis reserved for outside of grid, -1 on the x is reserved for unassigned cell
	Vector2 midPoint, origMidpoint;
	tag tag;
	//so that entities don't have their aabb's updated (i.e. transformations to scale and position is enacted) if they don't have any colliders in neighbouring cells.
	bool bMadeTrueAABB;
	Vector2 origAABB[2];//bottom-left, top-right
	Vector2 trueAABB[2];
	Entity* next;
	Entity* prev;
};
#define resetPos(transform) transform->updateVerticies = zeroBVector;
#define resetScale(transform) transform->matchesScale = oneBVector;
inline void ResetVertexUpdate(Transform* transform) {
	resetPos(transform);
}
inline void ResetScaleUpdate(Transform* transform) {
	resetScale(transform);
}
inline void ResetScaleNVertexUpdate(Transform* transform) {
	resetPos(transform);
	resetScale(transform);
}