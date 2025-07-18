#include <gl/glew.h>//4.6.13560 Compatibility Profile Context 26.20.11034.1006
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <Windows.h>
#include <stdbool.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <WinUser.h>
#define q 81U
#define left 37U
#define right 39U
#define control 17
#define shaderMaxLineChar 600U//this is the maximum number of characters a shader can have *PER LINE*. in final version change this to fit to each shader accordingly. this is not a problem if i exceed this when coding the shader, because i can change it before the alpha of the game comes out, AND the number of characters in the line with the most characters will not change from when the game is built to when it reaches the user's system.
#define shaderMaxChar 3000U//maximum number of characters shader will have *IN TOTAL*
#define frameWaitTime 1.0f / 75.0f * 1000.0f
int glError;
GLFWwindow* window = NULL;
GLFWvidmode* mode = NULL;
GLFWmonitor* monitor = NULL;
HWND windowHandle;
char** ss = NULL;
inline bool pressing(int key) {
	return key == -127 || key == -128;
}
typedef struct{
	int x;
	int y;
} Screen;
Screen screen;
const char* title = "small game";
int x = 1, y = 1, channels;
unsigned char *image = NULL;
void GlCall() {
	if ((glError = glGetError()) == GL_NO_ERROR) return;
	printf("ERROR:\n%s\n", glError);
}
typedef enum ShaderT {
	none = -1,
	vertex = 0,
	fragment = 1,
} ShaderType;
typedef struct ShaderProgramSrc {
	char* vertexSource;
	char* fragmentSource;
} ShaderProgramSource;
static ShaderProgramSource ParseShader(char* filePath) {
	FILE *fptr = NULL;
	fopen_s(&fptr, filePath, "r");
	if (fptr == NULL) {
		puts("cannot read file");
		DebugBreak();
	}
	char singleLine[shaderMaxLineChar];
	int i;
	const char* shader = "#shader ";
	const char* vertexStr = "vertex";
	const char* fragmentStr = "fragment";
	byte shaderLength = strnlen_s(shader, sizeof(shader));
	ss = malloc(sizeof(char*) * 2);
	ShaderType shaderType;
	USHORT lineLen;//declared outside of the while loop to lower initialization times so that the ushort isn't initialized every while iteration, at the small expense of it's memory being freed after the while loop at the end of the function.
	bool isSwitch;
	for (i = 0; i < 2; i++) {
		ss[i] = malloc(shaderMaxChar);
		*ss[i] = '\0';
	}
	shaderType = none;
	while (!feof(fptr)) {
		fgets(singleLine, shaderMaxLineChar, fptr);
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
		strcat_s(ss[shaderType], shaderMaxLineChar, singleLine);
		printf("concatenated to shader %d\n", shaderType);
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
		printf("%s", message);
		glDeleteShader(id);
		free(message);
		free(res);
		return 0;
	}
	return id;
}
static unsigned int CreateShader(const char* vertexShader, const char* fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);
	return program;
}
static inline void ThrowError(const char* message) {
	printf_s("%s\n", message);
	DebugBreak();
}
int Init() {//so that i can assign to stack and have it free on return to main() instead of assigning to the heap in main because ew very slow
	FILE* fptr;
	const char* path = "images/attack1/2.png";
	fptr = fopen(path, "r");
	if (fptr == NULL) ThrowError("fptr is null");
	fclose(fptr);
   	image = stbi_load(path, &x, &y, &channels, 4);
	return 0;
}
int /*WINAPI WinMain*/main()
{
	stbi_set_flip_vertically_on_load(1);
	Init();
	screen.x = GetSystemMetrics(0);
	screen.y = GetSystemMetrics(1);
	//*FPS = 75U;//this is what you edit to change the fps of the game
	//DWORD frameTime = (DWORD)(1.0f / *FPS * 1000.0f);
	//free(FPS);
	glfwInit();
	monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	window = glfwCreateWindow(screen.x, screen.y, title, monitor, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	if (glewInit() != GLEW_OK) ThrowError("your code is broken");
	windowHandle = glfwGetWin32Window(window);
	float positions[] = {
		-.5f, -.5f, .0f, .0f,
		.5f, -.5f, 1.0f, .0f,
		.5f, .5f, 1.0f, 1.0f,
		-.5f, .5f, .0f, 1.0f,
	};
	unsigned int indicies[] = {
		0, 1, 2,
		2, 3, 0
	};
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	printf("%s\n", glGetString(GL_VERSION));
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	GLint* texAttrib = malloc(sizeof(GLint));
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (const void*)0);
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_DYNAMIC_DRAW);
	char* filepath = NULL;
	rsize_t* maxLen = malloc(sizeof(rsize_t));
	size_t* len = malloc(sizeof(size_t));
	*maxLen = 20;
	*len = strnlen_s("Basic.shader", maxLen) + 1;
	filepath = malloc(sizeof(char) * *len);
	if (filepath == 0) {
		DebugBreak();
		return;
	}
	strncpy_s(filepath, sizeof(char) * *len, "Basic.shader", maxLen);
	free(len);
	free(maxLen);
	ShaderProgramSource shaderSrc = ParseShader(filepath);
	free(filepath);
	printf("vertex:\n%s\n", shaderSrc.vertexSource);
	printf("framgent:\n%s\n", shaderSrc.fragmentSource);
	unsigned int shader = CreateShader(shaderSrc.vertexSource, shaderSrc.fragmentSource);
	*texAttrib = glGetAttribLocation(shader, "texcoord");
	glEnableVertexAttribArray(*texAttrib);
	glVertexAttribPointer(*texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void*)(sizeof(float) * 2));
	glUseProgram(shader);
	int *color = glGetUniformLocation(shader, "u_Color");
	glUniform4f(color, .1f, .6f, .9f, 1.0f);
	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLuint texture;
	stbi_set_flip_vertically_on_load(1);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA8, GL_UNSIGNED_BYTE, image);
	glActiveTexture(GL_TEXTURE0);
	int* textureLocation = glGetUniformLocation(shader, "u_Texture");
	glUniform1i(textureLocation, 0);
	while (!glfwWindowShouldClose(window)) {
		if (pressing(GetKeyState(q)) && pressing(GetKeyState(control))) break;
		if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
			glfwPollEvents();
			Sleep(frameWaitTime);
			continue;
		}
		//37, 39
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader);
		//glUniform4f(location, .3f, .6f, .9f, 1.0f);
		glUniform4f(color, 1.0f, 1.0f, 1.0f, 1.0f);
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		glfwSwapBuffers(window);//calculate fps and use glfwSwapInterval for vsync
		glfwPollEvents();
		Sleep(frameWaitTime);
	}
	glDeleteTextures(1, &texture);
	stbi_image_free(image);
	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}