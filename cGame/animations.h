#pragma once
#include <GL/glew.h>
#include "main.h"
#include "Debug.h"
#define REQ_COMP 4
typedef union _animFrames animFrames;
typedef struct _Animator {
	ushort* animFrameNo;//number of frames in a given animation
	ushort animating, curAnimFrame;//i wanted to make these bytes but since they would have integer promotion for arithmatec it is less performant
	union _animFrames {
		uint** frameTextures;//not the texture itself; a uint that points to the texture in memory.
		Vector2*** texCoordPts;
	};
	uint* frameTexture;
	bool bUseTexCoordAnim;
	ushort noOfAnims;
	float animET;
	int textureID;
	bool active;//for when you want to disable the animator and still be able to free it later.
	float animSpeed;
	Vector2* animScales;
} Animator;
void InitAnimator(Animator* anim, char** paths, uint textureUnitIndex, char* wrapper);
void InitSingleTexture(char* path, uint textureUnitIndex, int textureID, uint *frameTex);//integer, then enumeration
void InitSingleTexture2(char* path, uint textureUnitIndex, int textureID);//integer, then enumeration
inline void ResetAnim(Animator* anim) {
	anim->curAnimFrame = -1;
}
inline void CheckResetAnim(Animator* anim, ushort check) {
	if (anim->animating == check) return;
	ResetAnim(anim);
}
inline bool FinishedAnim(Animator *animator, ushort animation) {
	return animator->animating != animation || animator->curAnimFrame == animator->animFrameNo[animation] - 1;
}
//stop index is the frame number at which the animation will stop playing
inline bool FinishedAnim2(Animator *animator, ushort animation, ushort stopIndex) {
	bool isAnim = animator->animating == animation;
	return animator->animating != animation || animator->curAnimFrame == stopIndex;
}
inline void StartAnim(Animator* animator, ushort animation) {
	CheckResetAnim(animator, animation);
	animator->animating = animation;
	animator->active = true;
}
inline void StartNonLoopAnim(Animator* animator, ushort animation) {
	animator->active = !FinishedAnim(animator, animation);
	CheckResetAnim(animator, animation);
	animator->animating = animation;
}
//doesn't care about reseting the animator
inline void StartNonLoopAnim2(Animator* animator, ushort animation) {
	CheckResetAnim(animator, animation);
	animator->active = !FinishedAnim(animator, animation);
	animator->animating = animation;
}
//stop index is the frame number at which the animation will stop playing
inline void StartNonLoopAnim3(Animator* animator, ushort animation, ushort stopIndex) {
	CheckResetAnim(animator, animation);
	animator->active = !FinishedAnim2(animator, animation, stopIndex);
	animator->animating = animation;
}