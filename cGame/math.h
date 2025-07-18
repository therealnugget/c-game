#pragma once
#include "Mesh.h"
#include <stdio.h>
#include "Debug.h"
#include "EntityManager.h"
#define lerp(aP, bP, time) aP * (1.0f - time + bP * time)
#define defInAABB(point, AABB) point.x >= AABB->x && point.y >= AABB->y && point.x <= AABB[1].x && point.y <= AABB[1].y
#define DEFAULT_POWER_MAX_ITER 100
#define DEFAULT_LOG_MAX_ITER 100
float Lerp(float pointA, float pointB, float t);
inline Vector2 VectorLerp(Vector2 vectorA, Vector2 vectorB, float t);
const float nugEpsilon;
inline bool InAABB(Vector2 point, Vector2* AABB) {//bottom-left, top-right
	return defInAABB(point, AABB);
}
//returns true only if the two colliders inputted are physically colliding; otherwise doesn't care about whether they are overlapping.
bool AABBsOverlap(Entity* collider, Entity* other);
inline float FAbs(float orig) {
	return orig * ((orig >= .0f) * 2.0f - 1.0f);
}
inline float FMax(float lhs, float rhs) {
	bool b = lhs >= rhs;
	return lhs * b + rhs * !b;
}
inline float FMin(float lhs, float rhs) {
	bool b = lhs <= rhs;
	return lhs * b + rhs * !b;
}
inline float FAvg(float lhs, float rhs) {
	return (lhs + rhs) / 2.0f;
}
inline int IAbs(int orig) {
	return orig * ((orig >= 0) * 2 - 1);
}
inline ULONGLONG UAbs(ULONGLONG orig) {
	return orig * ((ULONGLONG)(orig >= 0) * 2U - 1);
}
inline int IMax(int lhs, int rhs) {
	bool b = lhs >= rhs;
	return lhs * b + rhs * !b;
}
inline int IMin(int lhs, int rhs) {
	bool b = lhs <= rhs;
	return lhs * b + rhs * !b;
}
inline int IClamp(int in, int lower, int upper) {
	bool moreMax = in > upper;
	bool lessMin = in < lower;
	return in * (!moreMax && !lessMin) + upper * moreMax + lower * lessMin;
}
inline float FClamp(float in, float lower, float upper) {
	bool moreMax = in > upper;
	bool lessMin = in < lower;
	return in * (!moreMax && !lessMin) + upper * moreMax + lower * lessMin;
}
inline float Lerp(float a, float b, float t) {
	return a * (1.0f - t) + b * t;
}
inline float PowerFastF(float a, int b) {
	int origA = a;
	for (int i = 1; i < b; i++) {
		a *= origA;
	}
	return a;
}
inline float SquareF(float f) {
	return f * f;
}
inline float SignF(float f) {
	return (f > .0f) * 2.0f - 1.0f;
}
//exclusive min & max
inline bool InRange(float f, float min, float max) {
	return f < max && f > min;
}
inline bool InRangeInclMin(float f, float min, float max) {
	return f < max && f >= min;
}
inline bool InRangeInclMax(float f, float min, float max) {
	return f <= max && f > min;
}
inline bool InRangeInclusive(float f, float min, float max) {
	return f <= max && f >= min;
}
//exclusive min & max
inline bool I_InRange(int i, int min, int max) {
	return i < max && i > min;
}
inline bool I_InRangeInclMin(int i, int min, int max) {
	return i < max && i >= min;
}
inline bool I_InRangeInclMax(int i, int min, int max) {
	return i <= max && i > min;
}
inline bool I_InRangeInclusive(int i, int min, int max) {
	return i <= max && i >= min;
}
inline float BoolToSign(bool b) {
	return b * 2.f - 1.f;
}