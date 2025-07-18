#include "animations.h"
#include "GL stuff.h"
#include "Debug.h"
#include <stdlib.h>
#include "Dictionary.h"
#define ThrowImageError(){\
print("tried to load \"");\
print(path);\
print("\".\n");\
ThrowError(stbi_failure_reason());\
return;\
}
#define NULL_TERM_LEN 1
void InitAnimator(Animator* Anim, char** paths, uint textureUnitIndex, char* wrapper) {//don't call this other than from start. time complexity is ~ O(Anim->noOfAnims * curAnimFrameNo). also, this function should only be called **ONCE PER BATCH OF TEXTURES**. there is no difference if you initialize two animators with the same batch of textures other than an extremely drawn out startup time.
	const rsize_t bufferSize = sizeof(char) * 4;
	uint i;
	ushort maxPathNo = 0;
	ushort curLen;
	for (i = 0; i < Anim->noOfAnims; i++) if ((curLen = strlen(paths[i])) > maxPathNo) maxPathNo = curLen;
	char* images = "images/", *slash = "/", *pngExtension = ".png";
	const rsize_t pathSize = sizeof(char) * (strlen(images) + NULL_TERM_LEN + strlen(slash) + strlen(pngExtension) + strlen(wrapper) + maxPathNo) + bufferSize;
	char* path = malloc(pathSize);
	char* buffer = malloc(bufferSize);
	Anim->frameTextures = malloc(sizeof(uint*) * Anim->noOfAnims);//why do i barely use calloc? because i don't need the overhead of zeroing the memory when most of the time i don't need the memory zeroed. Maybe someday i will want that memory zeroed. (⚆_⚆)
	ushort curAnimFrameNo;
	byte** frames = malloc(sizeof(byte*) * Anim->noOfAnims);//outer pointer for holding groups of bytes which are in animations, inner pointers for holding the groups of images in each animation.
	glActiveTexture(Anim->textureID);
	int j;
	Anim->animET = .0f;
	Anim->curAnimFrame = -1;
	bool isNull;
	for (i = 0; i < Anim->noOfAnims; i++) {
		curAnimFrameNo = Anim->animFrameNo[i];
		frames[i] = malloc(sizeof(byte) * curAnimFrameNo);
		Anim->frameTextures[i] = malloc(sizeof(uint*) * curAnimFrameNo);
		glCreateTextures(GL_TEXTURE_2D, curAnimFrameNo, Anim->frameTextures[i]);
		for (j = 0; j < curAnimFrameNo; j++) {
			memset(path, 0, pathSize);
			strcpy_s(path, pathSize, images);
			if (wrapper) {
				strcat_s(path, pathSize, wrapper);
				strcat_s(path, pathSize, slash);
			}
			if (paths && paths[i]) {
				strcat_s(path, pathSize, paths[i]);
				strcat_s(path, pathSize, slash);
			}
			sprintf_s(buffer, bufferSize, "%d", j);
			strcat_s(path, pathSize, buffer);
			strcat_s(path, pathSize, pngExtension);
			frames[i] = TryGetFrame2(path, &isNull);
			if (!frames[i]) ThrowImageError();
			glBindTexture(GL_TEXTURE_2D, Anim->frameTextures[i][j]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frames[i]);
			if (isNull) glBindTextureUnit(textureUnitIndex, Anim->frameTextures[i][j]);
			//printf("binding texture unit index %u to texture %u.\n", textureUnitIndex, Anim->frameTextures[i][j]);
		}
	}
	glBindTexture(GL_TEXTURE_2D, Anim->frameTextures[0][0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, *frames);
}
void InitAnimator2(Animator* Anim, char** paths, uint textureUnitIndex, char* wrapper) {//don't call this other than from start. time complexity is ~ O(curAnimFrameNo * noOfAnims). also, this function should only be called **ONCE PER BATCH OF TEXTURES**. there is no difference if you initialize two animators with the same batch of textures other than an extremely drawn out startup time.
	const rsize_t bufferSize = sizeof(char) * 4;
	ushort i;
	ushort maxPathNo = 0;
	bool isLarger;
	ushort curLen;
	for (i = 0; i < Anim->noOfAnims; i++) {
		isLarger = (curLen = strlen(paths[i])) > maxPathNo;
		maxPathNo = maxPathNo * !isLarger + curLen * isLarger;
	}
	const rsize_t pathSize = sizeof(char) * (14 + strlen(wrapper) + maxPathNo) + bufferSize;
	char* path = malloc(pathSize);
	char* buffer = malloc(bufferSize);
	Anim->frameTextures = malloc(sizeof(uint*) * Anim->noOfAnims);//why do i barely use calloc? because i don't need the overhead of zeroing the memory when most of the time i don't need the memory zeroed. Maybe someday i will want that memory zeroed. (⚆_⚆)
	ushort curAnimFrameNo;
	byte** frames = malloc(sizeof(byte*) * Anim->noOfAnims);//outer pointer for holding groups of bytes which are in animations, inner pointers for holding the groups of images in each animation.
	glActiveTexture(Anim->textureID);
	int j;
	Anim->animET = .0f;
	Anim->curAnimFrame = -1;
	bool frameNull;
	for (i = 0; i < Anim->noOfAnims; i++) {
		curAnimFrameNo = Anim->animFrameNo[i];
		Anim->frameTextures[i] = malloc(sizeof(uint*) * curAnimFrameNo);
		glCreateTextures(GL_TEXTURE_2D, curAnimFrameNo, Anim->frameTextures[i]);
		for (j = 0; j < curAnimFrameNo; j++) {
			memset(path, 0, pathSize);
			strcpy_s(path, pathSize, "images/");
			if (wrapper) {
				strcat_s(path, pathSize, wrapper);
				strcat_s(path, pathSize, "/");
			}
			if (paths && paths[i]) {
				strcat_s(path, pathSize, paths[i]);
				strcat_s(path, pathSize, "/");
			}
			sprintf_s(buffer, bufferSize, "%d", j);
			strcat_s(path, pathSize, buffer);
			strcat_s(path, pathSize, ".png");
			frames[i] = TryGetFrame2(path, &frameNull);
			//frames[i] = stbi_load(path, &width, &height, &channels, REQ_COMP);
			if (!frames[i]) ThrowImageError();
			glBindTexture(GL_TEXTURE_2D, Anim->frameTextures[i][j]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frames[i]);
			if (frameNull) glBindTextureUnit(textureUnitIndex, Anim->frameTextures[i][j]);
			//printf("binding texture unit index %u to texture %u.\n", textureUnitIndex, Anim->frameTextures[i][j]);
		}
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, *frames);
}
//integer, then enumeration
void InitSingleTexture(char* path, uint textureUnitIndex, int textureID, uint **frameTex) {
	glActiveTexture(textureID);
	*frameTex = malloc(sizeof(uint));//don't free this. learnt my lesson the hard way. no non-semantic errors generated, but the texture loaded after making this mistake will be the texture of that of the function in which the first of this mistake was made sometimes. this comment is a bit hard to read i know, maybe try freeing this to see for yourself (me when hypocracy). additionally, the texture loaded after freeing this will have a different in-shader texture id, making this error VERY hard to solve and find reasoning for.
	glCreateTextures(GL_TEXTURE_2D, 1, *frameTex);
	//byte *frame = stbi_load(path, &width, &height, &channels, REQ_COMP);
	bool frameNull;
	byte* frame = TryGetFrame2(path, &frameNull);
	if (!frame) ThrowImageError();
	glBindTexture(GL_TEXTURE_2D, *frameTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame);
	if (frameNull) glBindTextureUnit(textureUnitIndex, *frameTex);
}
//integer, then enumeration
void InitSingleTexture2(char* path, uint textureUnitIndex, int textureID) {
	glActiveTexture(textureID);
	uint *frameTex = malloc(sizeof(uint));//don't free this. learnt my lesson the hard way. no non-semantic errors generated, but the texture loaded after making this mistake will be the texture of that of the function in which the first of this mistake was made sometimes. this comment is a bit hard to read i know, maybe try freeing this to see for yourself (me when hypocracy). additionally, the texture loaded after freeing this will have a different in-shader texture id, making this error VERY hard to solve and find reasoning for.
	glCreateTextures(GL_TEXTURE_2D, 1, frameTex);
	//byte *frame = stbi_load(path, &width, &height, &channels, 4);
	bool frameNull;
	byte *frame = TryGetFrame2(path, &frameNull);
	if (!frame) ThrowImageError();
	glBindTexture(GL_TEXTURE_2D, frameTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame);
	if (frameNull) glBindTextureUnit(textureUnitIndex, frameTex);
}