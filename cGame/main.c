#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include "GL stuff.h"//4.6.13560 Compatibility Profile Context 26.20.11034.1006
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "Player.h"
#include "files.h"
#include "Mesh.h"
#include "Debug.h"
#include "scene 1.h"
#include "EntityManager.h"
#include "vectors.h"
#include "math.h"
#include <io.h>
#include "Dictionary.h"
#include "math.h"
//#define RUN_IN_BACKGROUND
#define typeDefStruct(name, contents)\
typedef struct _##name{\
	contents\
} name;
#define typeDefUnion(name, contents)\
typedef union _##name{\
	contents\
} name;
#define CheckQuitStatus() if (PressingN_NotPressed(Q_KEY) && pressedControl) break;
#define CheckQuitKeyStatus()\
SetKey(Q_KEY);\
SetKey(VK_LCONTROL);\
SetKey(VK_RCONTROL)
#define CheckQuitPastKey()\
SetPastKey(Q_KEY);\
SetPastKey(VK_LCONTROL);\
SetPastKey(VK_RCONTROL)
#define RESUME_LATENCY 1
#define SAVE_FILE_MAX_LINE_CHAR 750
#define Idle(behav){\
glfwPollEvents();\
Sleep(longFrameWaitTime);\
notFocussed = true;\
behav;\
}
#define SetKey(key) pressingKey[key] = pressing(key)
#define SetPastKey(key) pressedKey[key] = pressing(key)
/*
*********************************************************************************************
*********************************************************************************************
*****************	ONCE BUILD IS FINISHED, FOR FINISHED VERSION, DISABLE  ******************
*****************					/RTC1 AND ENABLE /O2				   ******************
*********************************************************************************************
*********************************************************************************************
*/
char** ss = NULL;
float fullScreenET = 1.0f;
bool notFocussed;
float timeScale = 1.0f;
size_t vecSize = sizeof(Vector2);
inline bool pressing(int key) {
	return (GetKeyState(key) | 1) == -127;
}
#define NO_AVG_DT_FRAMES 10
#define RAND_ENTROPY_FACTOR 10
static float cumDT = .0f;//this stands for "cumulative delta time"
static float pastDT;
static float curDT;
bool pressingKey[NUM_KEYS] = { 0 };
bool pressedKey[NUM_KEYS] = { 0 };
typedef enum ShaderT {
	none = -1,
	vertex = 0,
	fragment = 1,
} ShaderType;
ShaderProgramSource ParseShader(char* filePath) {
	FILE *fptr = NULL;
	fopen_s(&fptr, filePath, "r");
	if (fptr == NULL) {
		puts("cannot read file");
		DebugBreak();
	}
	char singleLine[SHADER_MAX_LINE_CHAR];
	int i;
	const char* shader = "#shader ";
	const char* vertexStr = "vertex";
	const char* fragmentStr = "fragment";
	byte shaderLength = strnlen_s(shader, sizeof(shader));
	ss = malloc(sizeof(char*) * 2);
	ShaderType shaderType;
	USHORT lineLen;
	bool isSwitch;
	for (i = 0; i < 2; i++) {
		ss[i] = malloc(SHADER_MAX_CHAR);
		*ss[i] = '\0';
	}
	shaderType = none;
	while (!feof(fptr)) {
		fgets(singleLine, SHADER_MAX_LINE_CHAR, fptr);
		isSwitch = true;
		lineLen = strnlen_s(singleLine, sizeof(singleLine));
		for (i = 0; i < lineLen - 1|| i == 0; i++) {
			//printf("%d, %d, %d, %d|\n", singleLine[i] == shader[i], singleLine[i] == vertexStr[i - shaderLength], singleLine[i] == fragmentStr[i - shaderLength], i);
			if (singleLine[i] == shader[i]) {
				continue;
			}
			if (i == 0) {
				isSwitch = false;
				break;
			}
			if (singleLine[i] == vertexStr[i - shaderLength]) {
				shaderType = vertex;
				continue;
			}
			if (singleLine[i] == fragmentStr[i - shaderLength]) {
				shaderType = fragment;
				continue;
			}
			isSwitch = false;
			break;
		}
		if (isSwitch || shaderType == none) {
			puts("\n");
			continue;
		}
		strcat_s(ss[shaderType], SHADER_MAX_CHAR, singleLine);
	}
	ShaderProgramSource this;
	this.vertexSource = ss[0];
	this.fragmentSource = ss[1];
	return this;
}
static unsigned int CompileShader(unsigned int type, const char* src) {
	unsigned int id = glCreateShader(type);
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = malloc(sizeof(char) * length);
		glGetShaderInfoLog(id, length, &length, message);
		const char* t = type == GL_VERTEX_SHADER ? "vertex" : "fragment";
		char *res = NULL;
		char len = strnlen_s(t, sizeof(t)) + 1;
		res = malloc(sizeof(char) * len);
		strcpy_s(res, len, t);
		printf("failed to compile %s shader\n", res);
		puts(message);
		glDeleteShader(id);
		free(message);
		free(res);
		return 0;
	}
	return id;
}
unsigned int CreateShader(const char* vertexShader, const char* fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	int log;
	glGetProgramiv(program, GL_LINK_STATUS, &log);
	if (log == GL_FALSE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log);
		char* logInfo = malloc(sizeof(char) * log);
		glGetProgramInfoLog(program, log, NULL, logInfo);
		printf("shader is broken\n%s\n", logInfo);
		ThrowError("broken");
		return 0;
	}
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}
inline void ChangeFullScreen() {
	static bool fullScreen;
	glfwSetWindowMonitor(window, (fullScreen = !fullScreen) ? monitor : NULL, 0, 0, mode->width, mode->height, mode->refreshRate);
	glfwSwapInterval(vSyncCount);
	if (!fullScreen) return;
}
inline void SetFullScreen(bool fullScreen) {
	glfwSetWindowMonitor(window, fullScreen ? monitor : NULL, 0, 0, mode->width, mode->height, mode->refreshRate);
	glfwSwapInterval(vSyncCount);
}
static inline void OpenFileCheckError(wchar_t *path) {
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(path, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		ThrowError2("failed to create process.", GetLastError());
	}
}
static inline void OpenFileUnsafe(wchar_t *path) {
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	CreateProcess(path, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}
//takes in a pointer to a pointer to a pointer, and allocates size for the inner p-t-p
void AllocPtrPtr(void ***dst, size_t cnt) {
	*dst = malloc(sizeof(void **) * cnt);
}
void AllocNullStr(char** alloc, int strLen) {
	*alloc = malloc(sizeof(char) * strLen);
	**alloc = '\0';
}
bool pressedControl;
float startingFrame = (float)true;
int vSyncCount = 1;
#define SHOW_DEBUG_WINDOW 1
int 
#if !SHOW_DEBUG_WINDOW
WINAPI WinMain
#else
main
#endif
()
{
	stbi_set_flip_vertically_on_load(1);
	srand(time(NULL));
	{
		LARGE_INTEGER frequencyHolder;
		QueryPerformanceFrequency(&frequencyHolder);
		frequency = frequencyHolder.QuadPart;
		QueryPerformanceCounter(&pTime);
		char* save = ReadSaveFile("saves/save 1.txt", SAVE_FILE_MAX_LINE_CHAR);
		float deltaT = deltaTime();
		printf("\nLoading save file took %f seconds.\n", deltaT);
		free(save);
		GLenum hintTargets[4] = {
			GL_LINE_SMOOTH_HINT,
			GL_POLYGON_SMOOTH_HINT,
			GL_TEXTURE_COMPRESSION_HINT,
			GL_FRAGMENT_SHADER_DERIVATIVE_HINT,
		};
		ushort targetsLen = lengthOf(hintTargets), i;
		for (i = 0; i < targetsLen; i++) {
			glHint(hintTargets[i], GL_FASTEST);
			if (i >= RAND_ENTROPY_FACTOR) {
				continue;
			}
			rand();
		}
		if (targetsLen < RAND_ENTROPY_FACTOR) {
			for (i = 0; i < (RAND_ENTROPY_FACTOR - targetsLen); i++) {
				rand();
			}
		}
	}
	glfwInit();
	monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//glfwWindowHint(GLFW_MAXIMIZED, true);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_FALSE);
	{
		const char* title = "Hello, my name is Kevin, I have Changnesia. -- a reference from the popular show \"community\"";
		window = glfwCreateWindow(mode->width, mode->height, title, NULL, NULL);//need to startup with null for monitor parameter because otherwise window takes longer to start
	}
	glfwMakeContextCurrent(window);
	SetFullScreen(false);
	refreshRate = (float)mode->refreshRate;
	cumDT = (pastDT = frameTime = 1.0f / refreshRate * timeScale) * NO_AVG_DT_FRAMES;
	const DWORD longFrameWaitTime = (DWORD)(frameTime * 1000.0f) * RESUME_LATENCY;
	if (glewInit() != GLEW_OK) ThrowError("your program is broken");
	{
		GLint textureSlots;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureSlots);
		printf("%d texture slots are available on this machine.\n", textureSlots);
	}
	PhysicsInit();
	EntityInitialize();
	MeshInitialize();
	TerrainInit();
	SceneInit();
	PlayerInit();
	MeshStartup();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glUseProgram(0);
	//InitGPUTimeCheck();
	//printf("cellsize is (%f, %f).\n", cellSize.x, cellSize.y);
	while (!glfwWindowShouldClose(window)) {
		//StartGPUTimeCheck();
#ifndef RUN_IN_BACKGROUND
		if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) Idle(continue);
#endif
		if (!timeScale) Idle(\
			CheckQuitKeyStatus();\
			CheckQuitStatus();\
			CheckQuitPastKey();\
			goto cont
		);
		if (notFocussed) {
			QueryPerformanceCounter(&pTime);
			notFocussed = false;
		}
		//glUseProgram(shader);
		//glBindVertexArray(vao);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, curIndiciesLoc, GL_UNSIGNED_INT, NULL);
		glfwSwapBuffers(window);
		glfwPollEvents();
		SetKey(K_KEY);
		SetKey(J_KEY);
		SetKey(SPACE_KEY);
		SetKey(R_KEY);
		SetKey(VK_RETURN);
		SetKey(VK_MENU);
		CheckQuitKeyStatus();
		pressingKey[UP_ARROW] = pressing(UP_ARROW) || pressing(W_KEY);
		pressingKey[D_KEY] = pressing(D_KEY) || pressing(RIGHT_ARROW);
		pressingKey[A_KEY] = pressing(A_KEY) || pressing(LEFT_ARROW);
		pressingKey[VK_LSHIFT] = pressing(VK_LSHIFT) || pressing(VK_RSHIFT);
		startingFrame = (float)false;
		cumDT += (curDT = deltaTime()) - pastDT;
		pastDT = curDT;
		avgDT = cumDT / (float)NO_AVG_DT_FRAMES;
		//printf("%d\n", rand());
		PlayerUpdate();
		SceneUpdate();
		if (entityHead) UpdateEntities();//needs to be called AFTER every update which could have a regular transformation in it.
		if (squishHead) UpdateSquishables();//needs to be called AFTER every update which could have a squish transformation in it.
		if (sliderHead) UpdateSliders();//needs to be called AFTER every update which could have a slider transformation in it.
		/*sprintf(temp, "%f", deltaTime());
		strcat(temp, "\n");
		print(temp);
		temp[0] = '\0';*/
		//printf("deltatime is %f\n", deltaTime());
		if (fullScreenET > .2f)
		{
			if (IsHoldKey1PressKey2(VK_MENU, VK_RETURN)) {
				ChangeFullScreen();
				fullScreenET = 0;
			}
		}
		else fullScreenET += deltaTime();
		if (shouldMoveCam) {
			shouldMoveCam = false;
			glUniform2f(camPosLoc, camPosition.x, camPosition.y);
		}
		if (shouldDraw) {
			glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_DYNAMIC_DRAW);
			shouldDraw = false;
		}
		//Sleep(frameWaitTime);
		/*EndGPUTimeCheck();
		printf("took %f seconds\n", fabsf((float)(result * 1000000 / frequency)) / 1000000.0f);*/
		glCheckError();
	cont:
		pressedControl = PressingN_Pressed(VK_LCONTROL) || PressingN_Pressed(VK_RCONTROL);
		if (PressingN_NotPressed(R_KEY) && pressedControl) {
			OpenFileUnsafe(L"../x64/Release/cGame.exe");//no need for error checking if i just let this silently cause a bug, then i can execute both directories faster. Also, this is a billion times faster than a system call.
			OpenFileUnsafe(L"cGame.exe");
			break;
		}
		if (PressingN_NotPressed(Q_KEY) && pressedControl) break;
		if (pressing(VK_ESCAPE) && !pressedKey[VK_ESCAPE]) {
			PauseGame();
		}
		SetPastKey(R_KEY);
		SetPastKey(SPACE_KEY);
		SetPastKey(VK_ESCAPE);
		SetPastKey(VK_MENU);
		SetPastKey(VK_RETURN);
		pressedKey[UP_ARROW] = pressing(UP_ARROW) || pressing(W_KEY);
		CheckQuitPastKey();
		QueryPerformanceCounter(&pTime);
	}
	glDeleteProgram(shader);
	glfwTerminate();
	exit(0);
	return 0;
}