#pragma once
#include "Mesh.h"
#include "Physics.h"
#include "EntityManager.h"
void PlayerUpdate();
void PlayerInit();
void PlayerFinal();
typedef unsigned short ushort;
Entity player;
Slider hb_Bar;
bool playerFullyImmune;
float playerImmuneET;
bool plrGrounded;
PhysicsData* playerData;
float plrDmg;
EnumWithMax(PlrDmgType, projectileDmg, impactDmg);
bool* plrImmune;
inline void TakePlrDmg(float amount, PlrDmgType type){
	if (playerFullyImmune || plrImmune[type]) return;
	TakeSliderVal(hb_Bar, amount);
	playerFullyImmune = true;
	playerImmuneET = .0f;
}