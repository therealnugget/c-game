#include "Physics.h"
#include "main.h"
#include <stdio.h>
//yes, yes, i know, i shouldn't have made this THIS way.
LAYER_TYPE collisionMatrix[sizeof(LAYER_TYPE) * 8] = {
	1,//player
	2,//enemy
	-1,//platform
	11,//ground
	-1,//default
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
	-1,
};
float gravAccel = -9.5f;
/*
60 fps: -.43f
75 fps: 
144 fps: 
*/
//must be called before anything that uses "logrefreshrate", e.g. player init.
void PhysicsInit() {
	logRefreshRate = logf(refreshRate);
}