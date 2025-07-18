#pragma once
#include "EntityManager.h"
entityNode *SubscribeEntity(Entity* add);
squishList *SubscribeSquishable(SquishableMesh* add);
SliderList *SubscribeSlider(Slider* add);
void DeleteSlider(SliderList* toDelete);
void DeleteSquishable(squishList* toDelete);
entityNode *SubscribeEntity_Without_Scale(Entity* add);
void DeleteEntity(entityNode *toDelete, bool);
entityNode *CreateCollider(Vector2 AABB[2]);
entityNode *CreateCollider2(Vector2 AABB[2], int, int);
entityNode *CreateCollider3(Vector2 AABB[2], int, int, Vector2 offsetFrom);
void DestroyCollider(entityNode** col);
inline void RemoveSliderNode(SliderList* remove) {
	Remove2(remove, sliderHead, SliderList);
}