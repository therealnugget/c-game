#include "math.h"
#include "Mesh.h"
#include "Debug.h"
#include "EntityManager.h"
#include "vectors.h"
#include "Player.h"
#include "scene 1.h"
#include "main.h"
#include "Dictionary.h"
#define IsTag(_tag) (collider->tag == _tag || other->tag == _tag)
#define MAX 0
#define MIN 1
#define AVG 2
#define COR_TYPE MIN
#define CheckIntY(col1, col2) (col1->trueAABB[0].y > col2->trueAABB[0].y && col1->trueAABB[0].y < col2->trueAABB[1].y - x_int_tolerance_y)
#define CheckIntX(col1, col2) (col1->trueAABB[0].x < col2->trueAABB[0].x || col1->trueAABB[1].x > col2->trueAABB[1].x)
//no hashing function needed, as all pointers are unique. (e.g. no base ptr.)
#define FindMonkInd()\
monkIndex = GetUintVal2((uintptr_t)collider * colEnemy + (uintptr_t)other * !colEnemy)
const Vector2 upVector = { .0f, 1.0f }, downVector = { .0f, -1.0f }, rightVector = { 1.0f, .0f }, leftVector = { -1.0f, .0f };
const float nugEpsilon = .00001f;
typedef enum {
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
} direction;
const Vector2 fallBackVelocity = { .3f, .3f };
inline bool IsX(Vector2 v) {
	return FAbs(v.x) > FAbs(v.y);
}
typedef float (*floatDel)();
const float x_int_tolerance_y = .07f;
//returns true only if the two colliders inputted are physically colliding; otherwise return value doesn't care about whether they are overlapping.
bool AABBsOverlap(Entity* collider, Entity* other) {
	bool minMoreMinX = false, maxMoreMinX = false, minLessMaxX = false, maxLessMaxX = false, minMoreMinY = false, maxMoreMinY = false, minLessMaxY = false, maxLessMaxY = false;
	bool normalIsX = false;
	Vector2 normal = zeroVector;
	if ((minMoreMinX = collider->trueAABB[0].x > other->trueAABB[0].x || (maxMoreMinX = collider->trueAABB[1].x > other->trueAABB[0].x)) && (minLessMaxX = collider->trueAABB[0].x < other->trueAABB[1].x || (maxLessMaxX = collider->trueAABB[1].x < other->trueAABB[1].x)) && (minMoreMinY = collider->trueAABB[0].y > other->trueAABB[0].y || (maxMoreMinY = collider->trueAABB[1].y > other->trueAABB[0].y)) && (minLessMaxY = collider->trueAABB[0].y < other->trueAABB[1].y || (maxLessMaxY = collider->trueAABB[1].y < other->trueAABB[1].y))) {
		bool _this;
		PhysicsData* colData = collider->physicsData, * othData = other->physicsData;
		if (!IsLayer(collider->layer, other->layer) || !colData || !othData || colData->bIsTrigger || othData->bIsTrigger) {
			_this = false;
			goto collision_event;
		}
		_this = true;
		const float colNotStatic = !colData->bIsStatic;
		const float otherNotStatic = !othData->bIsStatic;
		const float colMult = (othData->bIsStatic + 1.0f) / 2.0f * colNotStatic, otherMult = (colData->bIsStatic + 1.0f) / 2.0f * otherNotStatic;
		const Vector2 fromTo = FromToVector(collider->midPoint, other->midPoint);
		const bool fromToIsX = IsX(fromTo);
		bool fromToIsRight = fromTo.x > .0f;
		bool fromToIsUp = fromTo.y > .0f;
		const Vector2 relativeVel = TakeVectors(othData->velocity, colData->velocity);
		const bool intX = (CheckIntX(collider, other) && CheckIntY(collider, other) || CheckIntX(other, collider) && CheckIntY(other, collider) || FAbs(NormalizedVector(fromTo).x) >= .6f);
		normal.x = ((fromToIsRight ? FMax : FMin)(collider->trueAABB[!fromToIsRight].x, other->trueAABB[!fromToIsRight].x) - (fromToIsX ? FMin : FMax)(collider->trueAABB[fromToIsRight].x, collider->trueAABB[fromToIsRight].x)) * intX;
		normal.y = ((fromToIsUp ? FMax : FMin)(collider->trueAABB[!fromToIsUp].y, other->trueAABB[!fromToIsUp].y) - (fromToIsUp ? FMin : FMax)(collider->trueAABB[fromToIsUp].y, collider->trueAABB[fromToIsUp].y)) * !intX;//positive overlap
		normalIsX = IsX(normal);
		const bool velIsX = IsX(relativeVel);
		Vector2 changed = { normalIsX && velIsX , !normalIsX && !velIsX };
		float dot = NormalDot(normal, relativeVel);
		float e =
#if COR_TYPE == MIN
			FMin
#elif COR_TYPE == MAX
			FMax
#else
			FAvg
#endif
			(colData->restitutionCoef, othData->restitutionCoef)
			;
		Vector2 jn = VectorByScalar(normal, (-1.0f - e) * dot / (colData->invMass + othData->invMass));
		VectorMultAssign(&jn, changed);
		if (dot > .0f) {
			TakeVectorsAssign(&colData->velocity, VectorByScalar(jn, colNotStatic));
			AddVectorsAssign(&othData->velocity, VectorByScalar(jn, otherNotStatic));
		}
		if (normalIsX) {
			if (colMult) ChangePositionXTo(colData->transform, colData->transform.position.x + normal.x * colMult);
			if (otherMult) ChangePositionXTo(othData->transform, othData->transform.position.x - normal.x * otherMult);
			goto update_aabb;
		}
		if (colMult) ChangePositionYTo(colData->transform, colData->transform.position.y + normal.y * colMult);
		if (otherMult) ChangePositionYTo(othData->transform, othData->transform.position.y - normal.y * otherMult);
	update_aabb:
		updateAABB2(curTrueAABB, collider, temp, colData);
		updateAABB2(curTrueAABB, other, temp2, othData);
		//semi-colon because of declaration beneath. i know this bool is always declared, but the compiler doesn't. (which kinda defeats the purpose of it throwing an error when i don't have the semi-colon there in the first place, since i could still not initialize the decleration.)
	collision_event:;
		const bool colEnemy = collider->tag == enemyTag;
		if (IsTag(playerTag)) {
			if (IsTag(enemyTag)) {
				uint monkIndex;
				FindMonkInd();
				float damage = enemyDamage[monkIndex];
				if (damage) TakePlrDmg(damage, impactDmg);
			}
			if (IsTag(groundTag) && !normalIsX && normal.y > .0f) {
				plrGrounded = true;
			}
		}
		if (IsTag(playerAttackTag) && (colEnemy || other->tag == enemyTag)) {
			//printf("%u\n", monkIndices[((uintptr_t)collider * colEnemy + (uintptr_t)other * othEnemy) % noOfEnemies]);
			uint monkIndex;
			FindMonkInd();
			if (!monkTakenDmg[monkIndex]) {
				Slider *slider = &enemyHBs[monkIndex].barSlider;
				TakeSliderVal(*slider, plrDmg);
				monkTakenDmg[monkIndex] = true;
			}
		}
		return _this;//the collision event will occur 20 times per frame unless coded otherwise, due to the re-iteration behaviour of the verlet intergration collision solver.
	}
	return false;
}