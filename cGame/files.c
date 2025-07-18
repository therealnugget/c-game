#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <Windows.h>
#include "Debug.h"
#include "main.h"
//char ptr returned needs to be freed after use.
char* ReadSaveFile(const char* src, const int maxLineChar) {
	FILE* fptr = NULL;
	fopen_s(&fptr, src, "r");
	if (!fptr) DebugBreak();
	rsize_t aSingleLineSize = sizeof(char) * maxLineChar;
	char* buffer;
	AllocNullStr(&buffer, aSingleLineSize);
	char* singleLine;
	AllocNullStr(&singleLine, aSingleLineSize);
	int singleLineStrLen, bufferStrlen;
	rsize_t curBufferSize = aSingleLineSize;
	while (!feof(fptr)) {
		fgets(singleLine, maxLineChar, fptr);
		singleLineStrLen = strnlen_s(singleLine, maxLineChar);
		bufferStrlen = strnlen_s(buffer, curBufferSize);
		curBufferSize = aSingleLineSize * sizeof(char) + bufferStrlen * sizeof(char) + sizeof(char);
		buffer = (char*)realloc(buffer, curBufferSize);
		strcat_s(buffer, curBufferSize, singleLine);
		singleLine = (char*)realloc(singleLine, sizeof(char) * singleLineStrLen + aSingleLineSize);
	}
	free(singleLine);
	fclose(fptr);
	return buffer;
}