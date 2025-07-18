#pragma once
#include "animations.h"
#include "EntityManager.h"
void SceneInit();
void TerrainInit();
void SceneUpdate();
Entity* platforms;
entityNode** platformNodes;
//contains position & scale because the struct is designed to be used as following an entity in world space.
typedef struct _HealthBar {
	Mesh borderMesh;
	Slider barSlider;
	Vector2 bordPosition, bordScale;
	Vector2 barPosition, barScale;
} HealthBar;
HealthBar* enemyHBs;
SliderList **monkHBNodes;
uint noOfEnemies;
bool* monkTakenDmg;
float* enemyDamage;