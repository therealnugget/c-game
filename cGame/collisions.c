#include "collisions.h"
#include "EntityManager.h"
void SetColliderAABB(Vector2 collider[NUM_VEC2_AABB], Vector2 offsets[NUM_VEC2_AABB], PhysicsData* physicsData) {
	for (int i = 0; i < NUM_VEC2_AABB; i++) {
		collider[i].x = physicsData->transform.position.x + offsets[i].x * physicsData->transform.scale.x;
		collider[i].y = physicsData->transform.position.y + offsets[i].y * physicsData->transform.scale.y;
	}
	checkGridScale3(collider);
}