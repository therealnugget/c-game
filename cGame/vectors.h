#pragma once
#include <stdbool.h>
#include <math.h>
#define Deconstruct(vec) (vec).x, (vec).y
#define BoolVecToStr(vec) BoolToStr(vec.x), BoolToStr(vec.y)
#define ZeroVecConst { .0f, .0f }
#define OneVecConst { 1.f, 1.f }
typedef struct {
	int width;
	int height;
} Rect;
typedef struct {
	float x;
	float y;
} Vector2;
typedef struct {
	int x;
	int y;
} intVector2;
typedef struct _bvec2 {
	bool x;
	bool y;
} boolVector2;
inline float Lerp(float a, float b, float t);
inline float FAbs(float orig);
inline float FMax(float a, float b);
inline float FMin(float a, float b);
Vector2 zeroVector;
Vector2 oneVector;
boolVector2 zeroBVector;
boolVector2 oneBVector;
size_t vecSize;
inline Vector2 AddVectors(Vector2 lhs, Vector2 rhs) {
	Vector2 _this;
	_this.x = lhs.x + rhs.x;
	_this.y = lhs.y + rhs.y;
	return _this;
}
inline Vector2 TakeVectors(Vector2 lhs, Vector2 rhs) {
	Vector2 _this;
	_this.x = lhs.x - rhs.x;
	_this.y = lhs.y - rhs.y;
	return _this;
}
inline Vector2 TakeVectorScalar(Vector2 lhs, float rhs) {
	Vector2 _this;
	_this.x = lhs.x - rhs;
	_this.y = lhs.y - rhs;
	return _this;
}
inline void AddVectorsAssign(Vector2* lhs, Vector2 rhs) {//the lhs will be the one added to
	*lhs = AddVectors(*lhs, rhs);
}
inline void TakeVectorsAssign(Vector2* lhs, Vector2 rhs) {//the lhs will be the one added to
	*lhs = TakeVectors(*lhs, rhs);
}
inline void TakeVectorScalarAssign(Vector2* lhs, float rhs) {//the lhs will be the one added to
	*lhs = TakeVectorScalar(*lhs, rhs);
}
inline Vector2 VectorByScalar(Vector2 vector, float scalar) {
	Vector2 _this = { vector.x * scalar, vector.y * scalar };
	return _this;
}
inline Vector2 GetVectorMidPoint2(Vector2 a, Vector2 b) {
	return VectorByScalar(AddVectors(a, b), .5f);
}
inline Vector2 GetVectorMidPoint(Vector2 vecs[2]) {
	return GetVectorMidPoint2(vecs[0], vecs[1]);
}
inline void VectorByScalarAss(Vector2 *vector, float scalar) {
	*vector = VectorByScalar(*vector, scalar);
}
inline Vector2 FromToVector(Vector2 from, Vector2 to) {
	Vector2 _this;
	_this.x = to.x - from.x;
	_this.y = to.y - from.y;
	return _this;
}
inline Vector2 NegVector(Vector2 vector) {
	Vector2 _this;
	_this.x = -vector.x;
	_this.y = -vector.y;
	return _this;
}
inline void NegVectorAssign(Vector2* vector) {
	*vector = NegVector(*vector);
}
inline Vector2 AbsVector(Vector2 vector) {
	Vector2 _this;
	_this.x = FAbs(vector.x);
	_this.y = FAbs(vector.y);
	return _this;
}
inline void AbsVectorAssign(Vector2 *vector) {
	*vector = AbsVector(*vector);
}
inline Vector2 NegAbsVector(Vector2 vector) {
	Vector2 _this;
	_this = NegVector(AbsVector(vector));
	return _this;
}
inline void NegAbsVectorAssign(Vector2 *vector) {
	*vector = NegAbsVector(*vector);
}
inline Vector2 VectorMult(Vector2 a, Vector2 b) {
	Vector2 _this;
	_this.x = a.x * b.x;
	_this.y = a.y * b.y;
	return _this;
}
inline void VectorMultAssign(Vector2* a, Vector2 b) {
	*a = VectorMult(*a, b);
}
inline Vector2 VectorByB_Vec(Vector2 a, boolVector2 b) {
	Vector2 _this = { a.x * b.x, a.y * b.y };
	return _this;
}
inline void VectorByB_VecAss(Vector2* a, boolVector2 b) {
	*a = VectorByB_Vec(*a, b);
}
inline float Magnitude(Vector2 vector) {
	return sqrtf(vector.x * vector.x + vector.y * vector.y);
}
inline float SqrMagnitude(Vector2 vector) {
	return vector.x * vector.x + vector.y * vector.y;
}//a dot a = SqrMagnitude(a), but dotting the vector with itself may incur more stack allocation and memory jumps if the compiler decides against inlining the dotproduct function.
inline Vector2 VectorDivByScalar(Vector2 a, float b) {
	Vector2 _this;
	_this.x = a.x / b;
	_this.y = a.y / b;
	return _this;
}
inline void VectorDivByScalarAssign(Vector2* a, float b) {
	*a = VectorDivByScalar(*a, b);
}
inline Vector2 ProjectedVector(Vector2 a, Vector2 b) {
	return VectorByScalar(b, SqrMagnitude(a) / SqrMagnitude(b));
}
inline void ProjectVector(Vector2* a, Vector2 b) {
	*a = ProjectedVector(*a, b);
}
inline Vector2 DivideVectors(Vector2 a, Vector2 b) {
	Vector2 _this;
	_this.x = a.x / b.x;
	_this.y = a.y / b.y;
	return _this;
}
inline void DivideVectorsAssign(Vector2* a, Vector2 b) {
	*a = DivideVectors(*a, b);
}
inline Vector2 NormalizedVector(Vector2 vector) {
	float magnitude = Magnitude(vector);
	if (magnitude) return VectorDivByScalar(vector, magnitude);
	return zeroVector;
}
inline void NormalizeVector(Vector2 *vector) {
	*vector = NormalizedVector(*vector);
}
const Vector2 upVector, downVector, rightVector, leftVector;
inline Vector2 VectorLerp(Vector2 vectorA, Vector2 vectorB, float t) {
	Vector2 _this = { Lerp(vectorA.x, vectorB.x, t), Lerp(vectorA.y, vectorB.y, t) };
	return _this;
}
inline float DotProduct(Vector2 LHS, Vector2 RHS) {
	return LHS.x * RHS.x + LHS.y * RHS.y;
}
inline float NormalDot(Vector2 LHS, Vector2 RHS) {
	NormalizeVector(&LHS);
	NormalizeVector(&RHS);
	return DotProduct(LHS, RHS);
}
inline Vector2 VectorMin(Vector2 a, Vector2 b) {
	Vector2 _this;
	_this.x = FMin(a.x, b.x);
	_this.y = FMin(a.y, b.y);
	return _this;
}
inline void MakeVectorMin(Vector2* a, Vector2 b) {
	*a = VectorMin(*a, b);
}
inline Vector2 VectorMax(Vector2 a, Vector2 b) {
	Vector2 _this;
	_this.x = FMax(a.x, b.x);
	_this.y = FMax(a.y, b.y);
	return _this;
}
inline void MakeVectorMax(Vector2* a, Vector2 b) {
	*a = VectorMax(*a, b);
}