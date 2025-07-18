#pragma once//not to be included in a non-debug build
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <stdio.h>
#define EndGPUTimeCheck() {\
	glEndQuery(GL_TIME_ELAPSED);\
	glGetQueryObjectiv(query, GL_QUERY_RESULT, &result);\
}//call after any gpu calls in main loop
#define Assert(b) if (!(b)){\
puts("assertion failed.\n");\
DebugBreak();\
}
#define Assert2(b, str) if (!(b)){\
puts("assertion failed: ");\
printf("%s\n", str);\
DebugBreak();\
}
//#define IGNORE_ERRORS//dangerous (to comment this out)? i don't know the meaning of the word! no, seriously, please someone tell me!
GLuint query;
GLint result;
int glError;
inline void InitGPUTimeCheck() {
	glGenQueries(1, &query);
}//call once before main loop
inline void StartGPUTimeCheck() {
	glBeginQuery(GL_TIME_ELAPSED, query);
}//call before any gpu calls in main loop
void glCheckError();
static inline void ThrowError(const char* message) {
	printf("%s\n", message);
#ifndef IGNORE_ERRORS
	DebugBreak();
#endif
}
static inline void ThrowError3() {
	printf("%s\n", "broken");
#ifndef IGNORE_ERRORS
	DebugBreak();
#endif
}
static inline void ThrowError2(const char* message, DWORD errorCode) {
	printf("%s\n", message);
	printf("error code was %lu.\n", errorCode);
#ifndef IGNORE_ERRORS
	DebugBreak();
#endif
}
void BreakProgram();
DWORD WINAPI printMultiThreaded(void* data);
inline void print(char* toPrint) {
	HANDLE thread = CreateThread(NULL, 0, printMultiThreaded, toPrint, 0, NULL);
	WaitForSingleObject(thread, INFINITE);
	CloseHandle(thread);
}