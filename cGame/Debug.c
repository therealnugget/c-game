#include "Debug.h"
#define SUCCESS 0
void glCheckError() {
	if ((glError = glGetError()) == GL_NO_ERROR) return;
	printf("ERROR %d:\n%s\n", glError, gluErrorString(glError));
	glCheckError();
}
DWORD WINAPI printMultiThreaded(void* data) {
	printf("%s", data);
	return SUCCESS;
}