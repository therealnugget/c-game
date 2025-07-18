#pragma once
#include <math.h>
#include "EntityManager.h"
//(a * seed + c) % m
//let (a, seed, c, m) = (22695477, seed, 12345, 2^31)
#ifndef mConst
#define mConst
const static unsigned long long maxNugRand = (unsigned long long)1U << 32;
#endif
#define DictData(structName, _structName, valueType, valueName)\
typedef struct _structName {\
	valueType valueName;\
	ULONGLONG hash;\
} structName
const unsigned long long aConst, cConst;
typedef struct _frameData {
	byte* frame;
	ULONGLONG hash;
} frameData;
typedef struct _locData {
	int value;
	ULONGLONG hash;
} locData;
DictData(uintData, _uintData, uint, value);
int TryGetLocValue(char *, char*);
int TryGetLocVal2(char *, bool *);
int GetLocVal(char *, ULONGLONG *);
int GetLocVal2(char *);
int AddLocVal(ULONGLONG, char *);
int AddLocVal2(char *);
uint TryGetUintVal(ULONGLONG path, uint attribName, uint value);
uint TryGetUintVal2(ULONGLONG path, bool* wasNull, uint value);
uint GetUintVal(ULONGLONG path, ULONGLONG* outHash);
uint GetUintVal2(ULONGLONG path);
uint AddUintVal(ULONGLONG hash, ULONGLONG key, uint value);
uint AddUintVal2(ULONGLONG key, uint value);
linkedList(frameData, frameList, _frameList);
linkedList(locData, locList, _locList);
linkedList(uintData, uintList, _uintList);
byte* AddFrameData(unsigned long long hash, char* path);
byte* GetFrame(char*, ULONGLONG *);
byte* TryGetFrame(char*);
byte* TryGetFrame2(char*, bool*);
//deterministic.
inline unsigned long long NugRand(unsigned long long seed) {
	return (aConst * seed + cConst) % maxNugRand;
}
long long NugHash(char*);
uintList** uintMap;