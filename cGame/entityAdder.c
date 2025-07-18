#include "entityAdder.h"
#include "EntityManager.h"
#include "vectors.h"
#include "math.h"
#include <math.h>
#include "scene 1.h"
#define SetOrigMidPoint(coord)\
add->origMidpoint.coord = (add->origAABB[1].coord + add->origAABB[0].coord) / 2.0f;\
if (data) add->origMidpoint.coord *= data->transform.scale.coord
#define Insert(add, valueToAdd, head)\
(add)->value = valueToAdd;\
(add)->next = head;\
(add)->prev = NULL;\
if (head) (head)->prev = add;\
(head) = add
#define DelEntNode(node)\
RemoveEntAppropriate((node)->entity);\
free((node)->entity);\
RemoveEntityNode(node, &entityHead);\
free(node)
void InsertAtMainHead(entityNode * add) {
	add->next = entityHead;
	add->prev = NULL;
	if (entityHead) entityHead->prev = add;
	entityHead = add;
	return add;
}
entityNode* SubscribeEntity(Entity* add) {
	entityNode* this = malloc(sizeof(entityNode));
	InsertAtMainHead(this);
	this->entity = add;
	PhysicsData* data = add->physicsData;
	checkGridScale(add);
	add->pastCellPosition.x = -1;
	add->origMidpoint = VectorDivByScalar(AddVectors(add->origAABB[1], *add->origAABB), 2.0f);
	if (data) VectorMultAssign(&add->origMidpoint, data->transform.scale);
	if (data) {
		data->transform.scale = oneVector;
		data->invMass = 1.0f / data->mass;
		data->transform.updateVerticies.x = true;//so that the sorting system sorts them on the first frame
		data->bIsStatic = false;
	}
	add->prev = add->next = NULL;
	noOfCellChecks++;
	nodeSection = IMax(noOfCellChecks / NUM_THREADS, 1);
	nodeExcess = noOfCellChecks % NUM_THREADS;
	endOfNodes = IMax(noOfCellChecks - nodeExcess, NUM_THREADS);
	totalCheckedCells = realloc(totalCheckedCells, sizeof(cellChecks) * noOfCellChecks);
	return this;
}
entityNode* SubscribeEntity_Without_Scale(Entity* add) {
	entityNode* this = malloc(sizeof(entityNode));
	InsertAtMainHead(this);
	this->entity = add;
	PhysicsData* data = add->physicsData;
	add->pastCellPosition.x = -1;
	add->origMidpoint = VectorDivByScalar(AddVectors(add->origAABB[1], *add->origAABB), 2.0f);
	if (data) VectorMultAssign(&add->origMidpoint, data->transform.scale);
	checkGridScale(add);
	if (data) {
		data->invMass = 1.0f / data->mass;
		data->transform.updateVerticies.x = true;
	}
	add->prev = add->next = NULL;
	noOfCellChecks++;
	nodeSection = IMax(noOfCellChecks / NUM_THREADS, 1);
	nodeExcess = noOfCellChecks % NUM_THREADS;
	endOfNodes = IMax(noOfCellChecks - nodeExcess, NUM_THREADS);
	totalCheckedCells = realloc(totalCheckedCells, sizeof(cellChecks) * noOfCellChecks);
	return this;
}
void DeleteEntity(entityNode* toDelete, bool allNull) {
	PhysicsData* data = toDelete->entity->physicsData;
	if (allNull) for (ushort i = 0; i < data->animator->noOfAnims; i++) glDeleteTextures(data->animator->animFrameNo[i], data->animator->frameTextures[i]);
	free(data->animator->animFrameNo);
	free(data->animator);
	free(data->mesh);
	free(data);
	DelEntNode(toDelete);
}
void DeleteSlider(SliderList *node) {
	DeleteSQuad(node->value->mesh);
	RemoveSliderNode(node);
	free(node);
}
void DeleteHealthbar(HealthBar* toDelete) {//TODO: make this function

}
void DeleteSquishable(squishList* toDelete) {//TODO: make this function
	return;
}
squishList* SubscribeSquishable(SquishableMesh* add) {
	squishList* _this = malloc(sizeof(squishList));
	add->origXMinY = vertices[add->meshLocation + stride];
	add->origXMaxY = vertices[add->meshLocation + stride * 2];
	_this->value = add;
	Insert(_this, add, squishHead);
	return _this;
}
SliderList* SubscribeSlider(Slider* add) {
	SliderList* _this = malloc(sizeof(SliderList));
	SquishableMesh* mesh = add->mesh;
	mesh->origXMinY = vertices[mesh->meshLocation + stride];
	mesh->origXMaxY = vertices[mesh->meshLocation + stride * 2];
	Insert(_this, add, sliderHead);
	return _this;
}
//non-physical, i.e. physicsData is NULL.
entityNode* CreateCollider(Vector2 AABB[2]) {
	Entity* entity = malloc(sizeof(Entity));
	rsize_t aabbSize = sizeof(Vector2) * AABB_NUM_POINTS;
	memcpy(entity->origAABB, AABB, aabbSize);
	memcpy(entity->trueAABB, entity->origAABB, aabbSize);
	entity->origMidpoint = VectorDivByScalar(AddVectors(AABB[1], *AABB), 2.0f);
	entity->physicsData = NULL;
	entity->pastCellPosition.x = -1;
	entityNode* this = SubscribeEntity(entity);
	return this;
}
entityNode* CreateCollider2(Vector2 AABB[2], int tag, int layer) {
	Entity* entity = malloc(sizeof(Entity));
	rsize_t aabbSize = sizeof(Vector2) * AABB_NUM_POINTS;
	memcpy(entity->origAABB, AABB, aabbSize);
	memcpy(entity->trueAABB, entity->origAABB, aabbSize);
	entity->origMidpoint = VectorDivByScalar(AddVectors(AABB[1], *AABB), 2.0f);
	entity->physicsData = NULL;
	entity->tag = tag;
	entity->layer = layer;
	entity->pastCellPosition.x = -1;
	entityNode* this = SubscribeEntity(entity);
	return this;
}
entityNode* CreateColAndSetPts(Vector2 AABB[2], int tag, int layer) {
	CreateCollider2(AABB, tag, layer);

}
entityNode* CreateCollider3(Vector2 AABB[2], int tag, int layer, Vector2 addToMidPt) {
	Entity* entity = malloc(sizeof(Entity));
	rsize_t aabbSize = sizeof(Vector2) * AABB_NUM_POINTS;
	memcpy(entity->origAABB, AABB, aabbSize);
	memcpy(entity->trueAABB, entity->origAABB, aabbSize);
	entity->midPoint = AddVectors(entity->origMidpoint = VectorDivByScalar(AddVectors(AABB[1], *AABB), 2.0f), addToMidPt);
	entity->physicsData = NULL;
	entity->tag = tag;
	entity->layer = layer;
	entity->pastCellPosition.x = -1;
	entityNode* this = SubscribeEntity(entity);
	return this;
}
void DestroyCollider(entityNode** col) {
	DelEntNode(*col);
	*col = NULL;
}