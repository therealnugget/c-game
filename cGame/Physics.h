#pragma once
#include "Mesh.h"
#include "main.h"
#include <math.h>
#define LAYER_TYPE long long
LAYER_TYPE collisionMatrix[sizeof(LAYER_TYPE) * 8];
inline bool IsLayer(uint layer1, uint layer2) {
	return collisionMatrix[layer1] & collisionMatrix[layer2];
}
void PhysicsInit();
float gravAccel;
typedef enum _layerName layerName;
float logRefreshRate;
enum _layerName {
	playerLayer = 0,
	enemyLayer = 1,
	dontColLayer = 2,
	groundLayer = 3,
	defaultLayer = 4,
};