#include <gl/glew.h>
#include "Mesh.h"
#include <corecrt_malloc.h>
#include <stdlib.h>
#include "Debug.h"
#include <stdarg.h>
#include <stdint.h>
#include "Dictionary.h"
#include "math.h"
#define TEX_COORD_X_OFFSET 2
//Mesh *CreateMesh(ushort noOfQuads) {
//	Mesh *this = malloc(sizeof(Mesh));
//	curMeshLoc += (this->meshLocation = (this->noOfVerticies = noOfQuads * lengthOf(positionTemplate)));
//	curIndiciesLoc += (this->indiciesLoc = (this->noOfIndicies = noOfQuads * lengthOf(indiciesTemplate)));
//	vertices = realloc(vertices, sizeof(vertices) + this->noOfVerticies * sizeof(float));
//	return this;
//}//this would be for meshes with multiple quads, but i don't need that yet
int textureIDs[32] = {
	0,
	1,
	2,
	3,
	4,
	5,
	6,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	17,
	18,
	19,
	20,
	21,
	22,
	23,
	24,
	25,
	26,
	27,
	28,
	29,
	30,
	31,
};
Mesh *CreateQuad() {
	Mesh *this = malloc(sizeof(Mesh));
	if (delMeshLen != 0) {
		this->meshLocation = delMesh[delMeshLen - 1];
		this->indiciesLoc = delInd[--delMeshLen];
		return this;
	}
	this->meshLocation = curMeshLoc;
	this->indiciesLoc = curIndiciesLoc;
	curMeshLoc += quadTempLen;
	curIndiciesLoc += indTempLen;
	vertices = realloc(vertices, (verticesSize += quadTempSize));
	indices = realloc(indices, (indicesSize += indTempSize));
	return this;
}
void CreateQuadAssign(Mesh *mesh) {
	if (delMeshLen != 0) {
		mesh->meshLocation = delMesh[delMeshLen - 1];
		mesh->indiciesLoc = delInd[--delMeshLen];
		return;
	}
	mesh->meshLocation = curMeshLoc;
	mesh->indiciesLoc = curIndiciesLoc;
	curMeshLoc += quadTempLen;
	curIndiciesLoc += indTempLen;
	vertices = realloc(vertices, (verticesSize += quadTempSize));
	indices = realloc(indices, (indicesSize += indTempSize));
}
void CreateSQuadAssign(SquishableMesh *mesh) {
	if (delMeshLen != 0) {
		mesh->meshLocation = delMesh[delMeshLen - 1];
		mesh->indiciesLoc = delInd[--delMeshLen];
		return;
	}
	mesh->meshLocation = curMeshLoc;
	mesh->indiciesLoc = curIndiciesLoc;
	curMeshLoc += quadTempLen;
	curIndiciesLoc += indTempLen;
	vertices = realloc(vertices, (verticesSize += quadTempSize));
	indices = realloc(indices, (indicesSize += indTempSize));
}
void DeleteQuad(Mesh *mesh) {
	uint meshLoc = mesh->meshLocation;
	memset(&vertices[meshLoc], 0, sizeof(float) * quadTempLen);
	delMeshSize += sizeof(uint);
	delMesh = realloc(delMesh, delMeshSize);
	delMesh[delMeshLen++] = meshLoc;
	delIndSize += sizeof(uint);
	delInd = realloc(delInd, delIndSize);
	delInd[delIndLen++] = mesh->indiciesLoc;
}
void DeleteSQuad(SquishableMesh *mesh) {
	uint meshLoc = mesh->meshLocation;
	memset(&vertices[meshLoc], 0, sizeof(float) * quadTempLen);
	delMeshSize += sizeof(uint);
	delMesh = realloc(delMesh, delMeshSize);
	delMesh[delMeshLen++] = meshLoc;
	delIndSize += sizeof(uint);
	delInd = realloc(delInd, delIndSize);
	delInd[delIndLen++] = mesh->indiciesLoc;
}
inline int GetOrigVertex(uint vertex) {
	return vertex - vertex / stride * (stride - 2);
}
//sets default mesh values, must be called before any mesh function or those functions won't work.
void MeshInitialize() {
	indTempSize = sizeof(indiciesTemplate);
	indTempLen = lengthOf(indiciesTemplate);
	posTempLen = lengthOf(positionTemplate);
	quadTempLen = lengthOf(quadTemplate);
	quadTempSize = sizeof(quadTemplate);
	strideTakeTwo = (stride = quadTempLen / 4) - 2;
}
//tells opengl the layout of everything, sets up vbo and ibo, loads shader, gives default values to shader. Needs mesh to be loaded into vertex and index array before being called otherwise the program will have a seizure and die
void MeshStartup() {
	printf("version %s\n", glGetString(GL_VERSION));
	uint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	size_t vertexSize = sizeof(quadTemplate) / 4;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vertexSize, (const void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, (const void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, vertexSize, (const void*)(sizeof(float) * 4));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, vertexSize, (const void*)(sizeof(float) * 8));
	uint ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_DYNAMIC_DRAW);
	char* filepath = "Basic.shader";
	ShaderProgramSource shaderSrc = ParseShader(filepath);
	printf("vertex:\n%s\n", shaderSrc.vertexSource);
	printf("framgent:\n%s\n", shaderSrc.fragmentSource);
	shader = CreateShader(shaderSrc.vertexSource, shaderSrc.fragmentSource);
	glUseProgram(shader);
	glUniform4f(glGetUniformLocation(shader, "u_Color"), .1f, .6f, .9f, 1.0f);
	glUniform1f(AddLocVal2("u_elapsedT"), .0f);
	glUniform1iv(glGetUniformLocation(shader, "u_Texture"), lengthOf(textureIDs), textureIDs);
	camPosLoc = glGetUniformLocation(shader, "u_cameraPos");
	glUniform2f(camPosLoc, camPosition.x, camPosition.y);
	shouldDraw = true;
}
void defaultMesh(Mesh* mesh, float textureUnitIndex)//IMPORTANT: if a mesh is created after the MeshStartup() function, the index buffer must be re-bound to ensure that the mesh renders.
{
	uint i, index;
	for (i = 0; i < quadTempLen; i++) {
		index = mesh->meshLocation + i;
		if ((i + 1) % stride == 0) {
			vertices[index] = textureUnitIndex;
			continue;
		}
		vertices[index] = quadTemplate[i];
	}
	uint indiciesLoc = mesh->indiciesLoc;
	for (i = 0; i < indTempLen; i++) indices[indiciesLoc + i] = indiciesTemplate[i] + indiciesLoc / 2 + indiciesLoc / indTempLen;
}
void ChangeTexInd(Mesh* mesh, float textureUnitIndex[4]){
	uint i, meshLoc = mesh->meshLocation;
	for (i = meshLoc + stride - 1; i < meshLoc + quadTempLen; i += stride) {
		vertices[i] = textureUnitIndex[(i - meshLoc + 1) / stride];
	}
}
void ChangeTexInd2(Mesh* mesh, float textureUnitIndex){
	uint i, meshLoc = mesh->meshLocation;
	for (i = meshLoc + stride - 1; i < meshLoc + quadTempLen; i += stride) {
		vertices[i] = textureUnitIndex;
	}
}
void defaultSMesh(SquishableMesh* mesh, float textureUnitIndex)//IMPORTANT: if a mesh is created after the MeshStartup() function, the index buffer must be re-bound to ensure that the mesh renders.
{
	uint i, index;
	for (i = 0; i < quadTempLen; i++) {
		index = mesh->meshLocation + i;
		if ((i + 1) % stride == 0) {
			vertices[index] = textureUnitIndex;
			continue;
		}
		vertices[index] = quadTemplate[i];
	}
	mesh->origXMinY = vertices[mesh->meshLocation + stride];
	mesh->origXMaxY = vertices[mesh->meshLocation + stride * 2];
	uint indiciesLoc = mesh->indiciesLoc;
	for (i = 0; i < indTempLen; i++) indices[indiciesLoc + i] = indiciesTemplate[i] + indiciesLoc / 2 + indiciesLoc / indTempLen;
}
void DefaultMeshNotPos(Mesh* mesh, float textureUnitIndex)//IMPORTANT: if a mesh is created after the MeshStartup() function, the index buffer must be re-bound to ensure that the mesh renders.
{
	uint i, index, iAddOne;
	for (i = 0; i < quadTempLen; i++) {
		iAddOne = i + 1;
		index = mesh->meshLocation + i;
		if (iAddOne % stride == 0) {
			vertices[index] = textureUnitIndex;
			continue;
		}
		if (((i % stride) & (iAddOne % stride)) == 0) continue;
		vertices[index] = quadTemplate[i];
	}
	for (i = 0; i < indTempLen; i++) indices[mesh->indiciesLoc + i] = indiciesTemplate[i] + mesh->indiciesLoc / 2 + mesh->indiciesLoc / indTempLen;
}
#define colorBoilerPlate uint isModFive, isModSix, isModSeven, backFour, backFive, backSix, backSeven;\
float index
#define colorBoilerPlate2(meshLocation) index = i - meshLocation;\
backFour = index - 4;\
backFive = index - 5;\
backSix = index - 6;\
backSeven = index - 7;\
isModFive = !(backFive % stride);\
isModSix = !(backSix % stride);\
isModSeven = !(backSeven % stride);\
vertices[i] = colors[!(backFour % stride) * backFour / stride + isModFive * backFive / stride + isModSix * backSix / stride + isModSeven * backSeven / stride][isModFive + isModSix * 2 + isModSeven * 3]
void defaultMeshWithColor(Mesh* mesh, float textureUnitIndex, float *colors[4])//IMPORTANT: if a mesh is created after the MeshStartup() function, the index buffer must be re-bound to ensure that the mesh renders.
{
	uint i;
	uint isModFive, isModSix, isModSeven, backFour, backFive, backSix, backSeven;
	uint index;
	for (i = mesh->meshLocation; i < mesh->meshLocation + quadTempLen; i++) {
		if (!((i + 1) % stride)) {
			vertices[i] = textureUnitIndex;
			continue;
		}
		if (!((i + 2) % stride) || !((i + 3) % stride) || !((i + 4) % stride) || !((i + 5) % stride)) {
			index = i - mesh->meshLocation;
			backFour = index - 4;
			backFive = index - 5;
			backSix = index - 6;
			backSeven = index - 7;
			isModFive = !(backFive % stride);
			isModSix = !(backSix % stride);
			isModSeven = !(backSeven % stride);
			vertices[i] = colors[!(backFour % stride) * backFour / stride + isModFive * backFive / stride + isModSix * backSix / stride + isModSeven * backSeven / stride][isModFive + isModSix * 2 + isModSeven * 3];
			continue;
		}
		index = i - mesh->meshLocation;
		vertices[i] = quadTemplate[index];
	}
	for (i = 0; i < indTempLen; i++) indices[mesh->indiciesLoc + i] = indiciesTemplate[i] + mesh->indiciesLoc / 2 + mesh->indiciesLoc / indTempLen;
}
void ChangeMeshColor(uint meshLoc, float* colors[]) {//TODO: test to see if this code works
	colorBoilerPlate;
	bool endOfColor
#ifdef _DEBUG
		= false;
#endif
		;
	for (uint i = meshLoc + 4; i < meshLoc + quadTempLen; i += (stride - 3) * (endOfColor = !((i + 2) % stride)) + !endOfColor) {
		colorBoilerPlate2(meshLoc);
	}
}
void SquishMeshX2(SquishableMesh *mesh, float amount) {
	float isStride;
	float notStride;
	float minVertX;
	uint meshLoc = mesh->meshLocation;
	for (uint i = meshLoc + stride; i < meshLoc + quadTempLen * 3 / 4; i += stride) {
		notStride = !(isStride = i - meshLoc == stride);
		minVertX = vertices[i + (uint)((-isStride + notStride) * stride)];
		vertices[i] = minVertX + (isStride * mesh->origXMinY + notStride * mesh->origXMaxY - minVertX) * (mesh->squishAmt = amount);
	}
}
void SquishMeshX(SquishableMesh *mesh) {
	float isStride;
	float notStride;
	float minVertX;
	uint meshLoc = mesh->meshLocation;
	for (uint i = meshLoc + stride; i < meshLoc + quadTempLen * 3 / 4; i += stride) {
		notStride = !(isStride = i - meshLoc == stride);
		minVertX = vertices[i + (uint)((-isStride + notStride) * stride)];
		vertices[i] = minVertX + (isStride * mesh->origXMinY + notStride * mesh->origXMaxY - minVertX) * mesh->squishAmt;
	}
}
float vecIndex(Vector2 vec, ushort index) {
	return vec.y * index + vec.x * (float)!index;
}
typedef struct {
	byte x;
	ushort y;
} uniqueAtlasStruct;
uniqueAtlasStruct atlasTexCoords[4] = {//x is point index, y is either x or y
	{0, 0},
	{0, 1},
	{1, 0},
	{1, 1},
};
byte atlasIndexBuffer[8] = {
	0,
	1,
	1,
	0,
	2,
	2,
	3,
	3,
};
void DefaultAtlasMesh(Mesh* mesh, float textureUnitIndex, Vector2 *points)//points must be normalized before their passing in; x's then y's for points.
{
	uint i, index;
	uniqueAtlasStruct curAtlasStruct;
	bool leftCheck, rightCheck;
	for (i = 0; i < quadTempLen; i++) {
		index = mesh->meshLocation + i;
		if ((i + 1) % stride == 0) {
			vertices[index] = textureUnitIndex;
			continue;
		}
		rightCheck = ((i - 3) % stride) == 0;
		leftCheck = ((i - 2) % stride) == 0;
		if (leftCheck || rightCheck) {
			curAtlasStruct = atlasTexCoords[atlasIndexBuffer[(i - 2 * leftCheck - 3 * rightCheck) / 9 + 4 * rightCheck]];
			vertices[index] = vecIndex(points[curAtlasStruct.x], curAtlasStruct.y);
			continue;
		}
		vertices[index] = quadTemplate[i];
	}
	for (i = 0; i < indTempLen; i++) indices[mesh->indiciesLoc + i] = indiciesTemplate[i] + mesh->indiciesLoc / 2 + mesh->indiciesLoc / indTempLen;
}
void DefaultAtlasMesh2(Mesh* mesh, float textureUnitIndex, Vector2 points1, Vector2 points2)//difference is that points are passed individually, as opposed to a vec2 ptr. points must be normalized before their passing in; x's then y's for points.
{
	uint i, index;
	uniqueAtlasStruct curAtlasStruct;
	bool leftCheck, rightCheck;
	bool isStructX;
	for (i = 0; i < quadTempLen; i++) {
		index = mesh->meshLocation + i;
		if ((i + 1) % stride == 0) {
			vertices[index] = textureUnitIndex;
			continue;
		}
		rightCheck = ((i - 3) % stride) == 0;
		leftCheck = ((i - 2) % stride) == 0;
		if (leftCheck || rightCheck) {
			curAtlasStruct = atlasTexCoords[atlasIndexBuffer[(i - 2 * leftCheck - 3 * rightCheck) / 9 + 4 * rightCheck]];
			isStructX = (bool)curAtlasStruct.x;
			vertices[index] = vecIndex(*(Vector2*)((uintptr_t)&points1 * isStructX + (uintptr_t)&points2 * !isStructX), curAtlasStruct.y);
			continue;
		}
		vertices[index] = quadTemplate[i];
	}
	for (i = 0; i < indTempLen; i++) indices[mesh->indiciesLoc + i] = indiciesTemplate[i] + mesh->indiciesLoc / 2 + mesh->indiciesLoc / indTempLen;
}
void MoveScaleMeshX(uint meshLoc, float toMoveTo, float scale) {
	for (vertexIterator = 0; vertexIterator < quadTempLen; vertexIterator += stride) vertices[meshLoc + vertexIterator] = positionTemplate[GetOrigVertex(vertexIterator)] * scale + toMoveTo;
}
void MoveScaleMeshY(uint meshLoc, float toMoveTo, float scale) {
	for (vertexIterator = 1; vertexIterator < quadTempLen; vertexIterator += stride) vertices[meshLoc + vertexIterator] = positionTemplate[GetOrigVertex(vertexIterator)] * scale + toMoveTo;
}
void MoveMeshX(uint meshLoc, float toMoveTo) {
	for (vertexIterator = 0; vertexIterator < quadTempLen; vertexIterator += stride) vertices[meshLoc + vertexIterator] = positionTemplate[GetOrigVertex(vertexIterator)] + toMoveTo;
}
void MoveMeshY(uint meshLoc, float toMoveTo) {
	for (vertexIterator = 1; vertexIterator < quadTempLen; vertexIterator += stride) vertices[meshLoc + vertexIterator] = positionTemplate[GetOrigVertex(vertexIterator)] + toMoveTo;
}
void MoveScaleMesh(uint meshLoc, Vector2 position, Vector2 scale) {
	float indRight = 0;
	float indLeft;
	uint strideTakeTwo = stride - 2;
	for (vertexIterator = 0; vertexIterator < quadTempLen; vertexIterator += (indRight * strideTakeTwo) + 1) {
		indRight = vertexIterator % stride;
		indLeft = !indRight;
		vertices[meshLoc + vertexIterator] = quadTemplate[vertexIterator] * (indLeft * scale.x + indRight * scale.y) + (indLeft * position.x + indRight * position.y);
	}
}
void MoveMeshTo(unsigned int meshLoc, Vector2 position) {
	float indRight;
	float indLeft;
	for (vertexIterator = 0; vertexIterator < quadTempLen; vertexIterator += (indRight * strideTakeTwo) + 1) {
		indRight = vertexIterator % stride;
		indLeft = !indRight;
		vertices[meshLoc + vertexIterator] = quadTemplate[vertexIterator] + (indLeft * position.x + indRight * position.y);
	}
}
void ScaleMesh(uint meshLoc, float scaleX, float scaleY) {
	char indRight = 0;
	for (vertexIterator = 0; vertexIterator < quadTempLen; vertexIterator += ((indRight = (vertexIterator) % stride) * strideTakeTwo) + 1) {
		vertices[meshLoc + vertexIterator] = quadTemplate[vertexIterator] * ((indRight ^ 1) * scaleX + indRight * scaleY);
	}
}
void ScaleMeshX(uint meshLoc, float scaleX) {
	for (vertexIterator = 0; vertexIterator < quadTempLen; vertexIterator += stride) vertices[meshLoc + vertexIterator] = quadTemplate[vertexIterator] * scaleX;
}
void ScaleMeshY(uint meshLoc, float scaleY) {
	char indRight = NULL;
	for (vertexIterator = 1; vertexIterator < quadTempLen; vertexIterator += stride) vertices[meshLoc + vertexIterator] = quadTemplate[vertexIterator] * scaleY;
}
//TODO: see if this works
void ChangeTexCoordTo(Mesh* mesh, Vector2* newVertices) {
	bool left = true;
	for (uint i = TEX_COORD_X_OFFSET; i < quadTempLen; i += ((left = !((i - (TEX_COORD_X_OFFSET + 1)) % stride)) * (stride - 2)) + 1) {
		vertices[i + mesh->meshLocation] = Branch(newVertices[I_InRange(i, stride - 1, stride * 3 - 1)].x, newVertices[i > (stride * 2 - 1)].y, left);
		//printf("assigned %u to %f, %s\n", i, vertices[i + mesh->meshLocation], BoolToStr(left));
	}
}
float positionTemplate[8] = {
	-.2604167f, -.462963f,
	.2604167f, -.462963f,
	.2604167f, .462963f,
	-.2604167f, .462963f,
};
float texcoordTemplate[8] = {
	.0f, .0f,
	1.0f, .0f,
	1.0f, 1.0f,
	.0f, 1.0f,
};
float colorTemplate[16] = {
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
};
float textureSlotTemplate[4] = {
	.0f,
	.0f,
	.0f,
	.0f,
};
float quadTemplate[36] = {
	-.2604167f, -.462963f, .0f, .0f, 1.0f, 1.0f, 1.0f, 1.0f, .0f,
	.2604167f, -.462963f, 1.0f, .0f, 1.0f, 1.0f, 1.0f, 1.0f, .0f,
	.2604167f, .462963f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, .0f,
	-.2604167f, .462963f, .0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, .0f,
};
uint indiciesTemplate[6] = {
	0, 1, 2,
	2, 3, 0,
};