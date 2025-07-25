#include <GL/glew.h>
#include "GL stuff.h"
#include <Windows.h>
#include "main.h"
#include "Mesh.h"
#include "Player.h"
#include <math.h>
#include "animations.h"
#include "entityAdder.h"
#include "math.h"
#include "Physics.h"
#include "scene 1.h"
//#define DEBUG_KEYS
//#define DEBUG_MESH_DATA
#define CAMERA_LERP_SPEED 10.0f
#define IDLE_TRANS_BUF .15f//I couldn't think of anything else to call it. The time buffer given from when you press shift to when you stop walking/running for the idle transition to still play
#define PLAYER_TEXTURE_ID GL_TEXTURE0
#define PLAYER_TEXTURE_INDEX 0
#define SHADER_PLR_TEX_ID .0f
#define HEALTHBAR_TEXTURE_ID GL_TEXTURE3
#define HEALTHBAR_TEXTURE_INDEX 3
#define SHADER_HB_TEX_ID 3.0f
#define HEALTHBAR_BAR_TEXTURE_ID GL_TEXTURE4
#define HEALTHBAR_BAR_TEXTURE_INDEX 4
#define SHADER_HB_BAR_TEX_ID 4.0f
#define SHOW_PLAYER TRUE
#define InitAnimatorAnimNo(animator, animNo)\
animator->noOfAnims = animNo;\
animator->animFrameNo = malloc(sizeof(ushort) * animator->noOfAnims)
#define playerHealth hb_Bar.mesh.squishAmt
//#define DEBUG_PLR_POS
#ifndef DEBUG_STUFF
#define DEBUG_STUFF
#define DEBUG_TEXTURE_ID GL_TEXTURE5
#define DEBUG_TEXTURE_INDEX 5
#define SHADER_DEBUG_TEX_ID 5.0f
#endif
#define ATTACK_1_OFFSET_X .02f
EnumWithMax(PlayerAnimation, idle, walk, run, idleTransition, jump, attack1);
#define PLAYER_NUM_ANIMS maxPlayerAnimation + 1
#define SetPtsAttk1(col) SetColliderAABB(col, attack1ColOffsets, playerData);
#define ATTACK_DMG_ANIM_INDEX 1
float playerSpeed, playerAccel = 16.f, origPlayerSpeed = .7f;
/*
player acceleration:
60 fps: .4
75 fps: .
144 fps: .7f
player orig speed:
60 fps: 
144 fps: 
*/
entityNode* playerEntity;
bool pressedMoveKey;
const static float immuneTime = .2f;
float jumpForce = 3.f;
/*
60 fps: 1.425f
75 fps: 
144 fps: 
*/
const float minJumpET = .2f;
/*
60 fps: .17
75 fps: 
144 fps:
*/
float jumpET;
float attack1MoveAmnt = 20.0f;
/*
144 fps: 47.0f
60 fps: 14.0f
*/
#ifdef DEBUG_PLR_POS
Mesh debugMesh[2];
Vector2 debugBoxSize;
#endif
Vector2 attack1ColOffsets[2] = { {-.12f, -.46f}, {.205f, .125f} };
static SliderList* playerHBNodes;//i used to add an underscore before a new capital after a capital letter, but i think it looks uglier than this -- this being the slightly less readable option.
Animator* plrAnim;
void PlayerInit() {
#ifdef DEBUG_PLR_POS
	debugBoxSize = VectorByScalar(oneVector, .03f);
#endif
	//origPlayerSpeed = -.007f * logRefreshRate + .0381f;//derived from excel -- not completely accurate.
	plrImmune = calloc(PlrDmgTypeLength, sizeof(bool));
	playerSpeed = origPlayerSpeed;
	playerAccel = 16.569736f * logRefreshRate - 19.7634973f;
	jumpET = minJumpET + 1.0f;
	playerData = (player.physicsData = malloc(sizeof(PhysicsData)));
	plrAnim = (playerData->animator = malloc(sizeof(Animator)));
	InitAnimatorAnimNo(plrAnim, PLAYER_NUM_ANIMS);
	plrAnim->animFrameNo[idle] = 7;
	plrAnim->animFrameNo[walk] = 7;
	plrAnim->animFrameNo[run] = 7;
	plrAnim->animFrameNo[idleTransition] = 2;
	plrAnim->animFrameNo[jump] = 3;
	plrAnim->animFrameNo[attack1] = 8;
	plrAnim->textureID = PLAYER_TEXTURE_ID;
	plrAnim->animSpeed = 1.0f;
	plrAnim->animScales = NULL;
	playerData->transform.position.y = -.01f;
	playerData->transform.position.x = .0f;
	playerData->transform.scale.x = 1.f;
	playerData->transform.scale.y = 1.f;
	char* paths[] = {
		"idle",
		"Walk",
		"run",
		"idle transition",
		"jump",
		"attack1",
	};
	InitAnimator(plrAnim, paths, PLAYER_TEXTURE_INDEX, "player");
	plrAnim->animating = idle;
	plrAnim->active = true;
	plrAnim->bUseTexCoordAnim = false;
	playerData->mesh = CreateQuad();//BATCH RENDERING FOR THE WIN \ (•◡•) /
#if SHOW_PLAYER
	defaultMesh(playerData->mesh, SHADER_PLR_TEX_ID);
#else
	float* colors[4] = {
		malloc(sizeof(float) * 4),
		malloc(sizeof(float) * 4),
		malloc(sizeof(float) * 4),
		malloc(sizeof(float) * 4),
	};
	float noAlpha[4] = { 1.0f, 1.0f, 1.0f, .0f };
	int i;
	for (i = 0; i < lengthOf(noAlpha); i++) colors[i] = noAlpha;
	defaultMeshWithColor(playerData->mesh, SHADER_PLR_TEX_ID, colors);
#endif
	playerData->gravityMultiplier = 1.0f;
	playerData->velocity.y = playerData->velocity.x = .0f;
	player.origAABB->x = -.037977435f;
	player.origAABB->y = -.462963f;
	player.origAABB[1].x = .037977435f;
	player.origAABB[1].y = -.02204586f;
	playerData->restitutionCoef = .06f;
	playerData->frictionCoef.x = .005f;
	playerData->frictionCoef.y = .005f;
	player.tag = playerTag;
	player.layer = playerLayer;
	playerData->bIsTrigger = false;
	playerData->mass = 1.0f;
	playerEntity = SubscribeEntity(&player);
	Mesh hb_Mesh;
	CreateQuadAssign(&hb_Mesh);
	defaultMesh(&hb_Mesh, SHADER_HB_TEX_ID);
	InitSingleTexture2("images/Retro Inventory/Original/Health_01.png", HEALTHBAR_TEXTURE_INDEX, HEALTHBAR_TEXTURE_ID);
	Vector2 hb_Pos = { -.73f, .85f };
	Vector2 hb_Scale = { 1.0f, .26f };
	MoveScaleMesh(hb_Mesh.meshLocation, hb_Pos, hb_Scale);
	DefaultSMesh(hb_Bar, 1.0f, 4.f, .001f);
	CreateSQuadAssign(hb_Bar.mesh);
	defaultSMesh(hb_Bar.mesh, SHADER_HB_BAR_TEX_ID);
	InitSingleTexture2("images/Retro Inventory/Original/Health_01_Bar01.png", HEALTHBAR_BAR_TEXTURE_INDEX, HEALTHBAR_BAR_TEXTURE_ID);
	Vector2 hb_Bar_Pos = { -.6770813f, .88f };
	Vector2 hb_Bar_Scale = { .702f, .0331f };
	MoveScaleMesh(hb_Bar.mesh->meshLocation, hb_Bar_Pos, hb_Bar_Scale);
	SquishableMesh* mesh = hb_Bar.mesh;
	mesh->squishAmt = 1.0f;
	mesh->bSquish = false;
	playerHBNodes = SubscribeSlider(&hb_Bar);
	playerImmuneET = immuneTime + 1.0f;
#ifdef DEBUG_PLR_POS
	float** colors[MESH_COLORS_LEN];
	uint i;
	uint j;
	for (i = 0; i < 2; i++) {
		colors[i] = malloc(sizeof(float*) * MESH_COLORS_LEN);
		for (j = 0; j < MESH_COLORS_LEN; j++) {
			colors[i][j] = calloc(MESH_NUM_COLORS, sizeof(float));
			colors[i][j][i] = colors[i][j][3] = 1.0f;
		}
	}
	Vector2 positions[2] = {AddVectors(*player.origAABB, *attack1ColOffsets), AddVectors(player.origAABB[1], attack1ColOffsets[1])};
	DefaultNewObjectsColor2(SHADER_DEBUG_TEX_ID, "images/debug/0.png", DEBUG_TEXTURE_INDEX, DEBUG_TEXTURE_ID, positions, debugBoxSize, colors, debugMesh, 2);
#endif
	//attack1MoveAmnt = 0.3929f * refreshRate - 9.5714f;
	playerData = player.physicsData;
}
void PlayerFinalize() {
	DeleteEntity(playerEntity, true);
	for (ushort i = 0; i < playerData->animator->noOfAnims; i++) glDeleteTextures(playerData->animator->animFrameNo[i], playerData->animator->frameTextures[i]);
}
Vector2 negPlrPos;
bool movingX;
float scaleX;
bool jumping, jumped, lastFrmJmp, nLastFrmJmp = true;
bool plrGrounded = true, groundedLastFrm;
float plrDmg = .5f;
float attk1MoveAmnts[] = {
	.0f,
	.0f,
	.04882813125f,
	.0054253479167f,
	.0108506958333f,
	.0f,
	.0f,
	.0f,
};
bool animWalk;
Vector2 attack1Col[NUM_VEC2_AABB];
static entityNode* attack1ColNode;
static Entity* attack1Ent;
static bool frm1LstFrm;
inline void TryDestAttk1() {
	if (!attack1ColNode) return;
	DestroyCollider(&attack1ColNode);
}
//the factor of spd that running adds. (i.e., if the player is sprinting their spd will be (1 + [this var]) multiplied by their original spd.
const float runSpdAddFact = 2.f;
static bool idleTransImminent;
static bool PlrFinAnim(int animation) {
	return FinishedAnim(plrAnim, animation);
}
void PlayerUpdate() {
#ifdef DEBUG_KEYS
	for (int i = 0; i < NUM_KEYS; i++) if (pressing(i)) printf("%d\n", i);
#endif
#ifdef DEBUG_MESH_DATA
	uint iAddOne;
	for (uint i = 0; i < verticesSize / sizeof(verticesSize); i++) {
		iAddOne = i + 1;
		printf("%f", vertices[i]);
		if (iAddOne != stride) printf("%s", ", ");
		if (!(iAddOne % quadTempLen)) puts("\n");
		if (!(iAddOne % stride)) puts("\n");
	}
	puts("\n");
#endif
	if (movingX = (pressingKey[D_KEY] ^ pressingKey[A_KEY])) {
		pressedMoveKey = true;
		scaleX = pressingKey[D_KEY] * 2.0f - 1.0f;
		playerSpeed = origPlayerSpeed * (1.f + runSpdAddFact * (pressingKey[VK_RSHIFT] || pressingKey[VK_LSHIFT]));
		AddForceScaleLimitVelX(*playerData, playerData->transform, avgDT * playerAccel * scaleX, scaleX, playerSpeed);
		if (pressingKey[VK_LSHIFT]) {
			idleTransImminent = true;
		}
		if (attack1ColNode) {
			SetPtsAttk1(attack1Ent->trueAABB);
		}
	}
	jumped = (jumped || lastFrmJmp) && !plrGrounded;
	if ((jumping = plrGrounded && pressingKey[UP_ARROW])) {
		AddForceY(*playerData, playerData->transform, jumpForce);
	}
#ifdef DEBUG_PLR_POS
	for (int i = 0; i < lengthOf(debugMesh); i++) MoveScaleMesh(debugMesh[i].meshLocation, player.trueAABB[i], debugBoxSize);
#endif
	idleTransImminent |= plrAnim->animating == run;
	if (!plrGrounded) {
		plrAnim->curAnimFrame *= nLastFrmJmp;
		StartNonLoopAnim3(plrAnim, jump, plrAnim->animFrameNo[jump] - 1 - jumped);
		if (!jumped) {
			plrAnim->curAnimFrame = plrAnim->animFrameNo[jump] - 2;
			plrAnim->animET = ANIM_UPDATE_TIME;
		}
		plrAnim->animET *= nLastFrmJmp;
	}
	else if (pressingKey[SPACE_KEY] || !PlrFinAnim(attack1)) {
		StartAnim(plrAnim, attack1);
		if (!movingX) {
			AddForceLimitVelX(*playerData, playerData->transform, attk1MoveAmnts[plrAnim->curAnimFrame] * deltaTime() * attack1MoveAmnt * playerData->transform.scale.x, playerSpeed);
		}
		if (plrAnim->curAnimFrame == ATTACK_DMG_ANIM_INDEX) {
			if (!attack1ColNode) {
				SetPtsAttk1(attack1Col);
				attack1Ent = (attack1ColNode = CreateCollider2(attack1Col, playerAttackTag, dontColLayer))->entity;
			}
			if (!frm1LstFrm) memset(monkTakenDmg, 0, sizeof(bool) * noOfEnemies);
		}
		else TryDestAttk1();
	}
	else {
		TryDestAttk1();
		if (movingX) {
			StartAnim(plrAnim, walk + pressingKey[VK_LSHIFT]);
		}
		else if (idleTransImminent && (plrAnim->curAnimFrame != plrAnim->animFrameNo[idleTransition] - 1 || plrAnim->animating != idleTransition)) {
			StartAnim(plrAnim, idleTransition);
		}
		else {
			StartAnim(plrAnim, idle);
			idleTransImminent = false;
		}
	}
#define CamLerp(coord) Lerp(camPosition.coord, negPlrPos.coord, limitedDeltaTime() * CAMERA_LERP_SPEED)
	negPlrPos = NegVector(playerData->transform.position);
	if (SqrMagnitude(AddVectors(camPosition, playerData->transform.position)) >= nugEpsilon) {
		ChangeCameraPosition(CamLerp(x), CamLerp(y));
	}
	if (playerImmuneET < immuneTime) {
		playerImmuneET += deltaTime();
	}
	else {
		playerFullyImmune = false;
	}/*
	printf("playing the %uth animation @ frame %u\n", plrAnim->animating, plrAnim->curAnimFrame);
	printf("the player is %sgrouned.\n", plrGrounded ? "" : "not ");*/
	nLastFrmJmp = !(lastFrmJmp = jumping);
	frm1LstFrm = plrAnim->curAnimFrame == 1;
	groundedLastFrm = plrGrounded;
}