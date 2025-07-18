#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "EntityManager.h"
#include "Dictionary.h"
#include "main.h"
#include "scene 1.h"
#include "math.h"
//what an absolute cesspool of code this script is! although, it's c, so it basically needs to be this way.
//increase once needed.
#define FRAME_DIC_SIZE 100
#define LOC_DIC_SIZE 100
//from the following definition to "AddVal2" is only for lists which have a value type that isn't a pointer pointing to data that needs to be populated.
#define TryGetValue(valueType, nameOfMap, dictionarySize, funcName, addValName, GetValName, typeOfKey)\
valueType funcName(typeOfKey path, typeOfKey attribName) {\
	ULONGLONG hash;\
	valueType frame = GetValName(path, &hash, attribName);\
	if (!frame) {\
		return addValName(hash, path);\
	}\
	return frame;\
}
#define TryGetVal2(valueType, nameOfMap, dictionarySize, funcName, GetValName, addValName, typeOfKey)\
valueType funcName(typeOfKey path, bool* wasNull) {\
	ULONGLONG hash;\
	valueType frame = GetValName(path, &hash);\
	if (*wasNull = !frame) {\
		return addValName(hash, path);\
	}\
	return frame;\
}
#define GetValue(valueType, nameOfMap, dictionarySize, funcName, typeOfList, typeOfKey)\
valueType funcName(typeOfKey path, ULONGLONG* outHash) {\
	ULONGLONG hash = NugHash(path);\
	typeOfList* head = nameOfMap[hash % dictionarySize];\
	while (head) {\
		if (head->value->hash == hash) return head->value->value;\
		advance(head);\
	}\
	*outHash = hash;\
	return NULL;\
}
#define GetVal2(valueType, nameOfMap, dictionarySize, funcName, typeOfList, typeOfKey)\
valueType funcName(typeOfKey path) {\
	ULONGLONG hash = NugHash(path);\
	typeOfList* head = nameOfMap[hash % dictionarySize];\
	while (head) {\
		if (head->value->hash == hash) return head->value->value;\
		advance(head);\
	}\
	return NULL;\
}
#define AddValue(valueType, nameOfMap, dictionarySize, funcName, typeOfList, valToCalc, typeOfkey)\
valueType funcName(ULONGLONG hash, typeOfkey attribName) {\
	typeOfList* list = malloc(sizeof(typeOfList));\
	list->value = malloc(sizeof(*list->value));\
	list->value->value = valToCalc;\
	list->value->hash = hash;\
	typeOfList** head = &nameOfMap[hash % dictionarySize];\
	Insert(list, head);\
	return list->value->value;\
}
#define AddVal2(valueType, nameOfMap, dictionarySize, funcName, typeOfList, valToCalc, typeOfKey)\
valueType funcName(typeOfKey attribName) {\
	typeOfList* list = malloc(sizeof(typeOfList));\
	list->value = malloc(sizeof(*list->value));\
	list->value->value = valToCalc;\
	ULONGLONG hash = NugHash(attribName);\
	list->value->hash = hash;\
	typeOfList** head = &nameOfMap[hash % dictionarySize];\
	Insert(list, head);\
	return list->value->value;\
}
//the following definition is only for lists which have a value type that isn't a pointer pointing to data that needs to be populated.
#define DictionaryFuncs(valueType, nameOfMap, dictionarySize, tryGetValName, tryGetVal2Name, GetVal2Name, GetValueName, AddValueName, AddVal2Name, typeOfList, valToCalc, typeOfKey)\
TryGetValue(valueType, nameOfMap, dictionarySize, tryGetValName, AddValueName, GetValueName, typeOfKey);\
TryGetVal2(valueType, nameOfMap, dictionarySize, tryGetVal2Name, tryGetValName, AddValueName, typeOfKey);\
GetValue(valueType, nameOfMap, dictionarySize, GetValueName, typeOfList, typeOfKey);\
GetVal2(valueType, nameOfMap, dictionarySize, GetVal2Name, typeOfList, typeOfKey);\
AddValue(valueType, nameOfMap, dictionarySize, AddValueName, typeOfList, valToCalc, typeOfKey);\
AddVal2(valueType, nameOfMap, dictionarySize, AddVal2Name, typeOfList, valToCalc, typeOfKey)
const ULONGLONG cConst = 12345, aConst = 22695477;
frameList* frameMap[FRAME_DIC_SIZE] = { 0 };
locList* locMap[LOC_DIC_SIZE] = { 0 };
uintList** uintMap = { 0 };
uint GetUintVal2(ULONGLONG key) {
	ULONGLONG hash = NugRand(key);
	uintList* head = uintMap[UAbs(hash % noOfEnemies)];
	while (head) {
		if (head->value->hash == hash) {
			return head->value->value;
		}
		head = head->next;
	}
	DebugBreak();
	return ((void*)0);
};
uint AddUintVal2(ULONGLONG key, uint value) {
	uintList* list = malloc(sizeof(uintList));
	list->value = malloc(sizeof(*list->value));
	list->value->value = value;
	ULONGLONG hash = NugRand(key);
	list->value->hash = hash;
	uintList** head = &uintMap[UAbs(hash % noOfEnemies)];
	list->next = *head;
	list->prev = ((void*)0);
	if (*head) (*head)->prev = list;
	*head = list;
	return list->value->value;
};
int GetLocVal2(char* path) {
	ULONGLONG hash = NugHash(path);
	locList* head = locMap[hash % 100];
	while (head) {
		if (head->value->hash == hash) return head->value->value;
	cont:
		head = head->next;
	}
	return NULL;
};
int AddLocVal2(char* attribName) {
	locList* list = malloc(sizeof(locList));
	list->value = malloc(sizeof(locData));
	list->value->value = glGetUniformLocation(shader, attribName);
	ULONGLONG hash = NugHash(attribName);
	list->value->hash = hash;
	locList** head = &locMap[hash % LOC_DIC_SIZE];
	list->next = *head;
	list->prev = NULL;
	if (*head) (*head)->prev = list;
	*head = list;
	return list->value->value;
};
byte* AddFrameData(ULONGLONG hash, char* path) {//always adds; doesn't get.
	frameList* list = malloc(sizeof(frameList));
	list->value = malloc(sizeof(frameData));
	list->value->frame = malloc(sizeof(byte));
	byte* frame = stbi_load(path, &width, &height, &channels, REQ_COMP);
	list->value->frame = frame;
	list->value->hash = hash;
	frameList** head = &frameMap[hash % FRAME_DIC_SIZE];
	Insert(list, head);
	return frame;
}
byte* TryGetFrame(char *path) {//only use if you are unsure if the value is in the map. kinda slow because of the linear probing. will add to the map if the key isn't already in it.
	ULONGLONG hash;
	byte* frame = GetFrame(path, &hash);
	if (!frame) {
		//printf("returned new frame for \"%s\"\n", path);
		return AddFrameData(hash, path);
	}
	return frame;
}
byte* TryGetFrame2(char *path, bool *wasNull) {//only use if you are unsure if the value is in the map. kinda slow because of the linear probing. will add to the map if the key isn't already in it.
	ULONGLONG hash;
	byte* frame = GetFrame(path, &hash);
	if (*wasNull = !frame) {
		//printf("returned new frame for \"%s\"\n", path);
		return AddFrameData(hash, path);
	}
	//printf("returned %zu for path \"%s\"\n", frame, path);
	return frame;
}
byte* GetFrame(char *path, ULONGLONG *outHash) {//only use if you are sure if the value is in the map. as slow as TryGetFrame, but doesn't add to the map if the key isn't found. instead, will return null if the key is not found.
	ULONGLONG hash = NugHash(path);
	frameList* head = frameMap[hash % FRAME_DIC_SIZE];
	while (head) {
		if (head->value->hash == hash) return head->value->frame;
		advance(head);
	}
	*outHash = hash;
	return NULL;
}
//deterministic.
ULONGLONG NugHash(char* string) {
	long long _this = 0;
	uint i = 0;
	while (*++string) {
		_this += NugRand(*string) * (i + 1);
		i++;
	}
	return _this;
}