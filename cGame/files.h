#pragma once
char* ReadSaveFile(char* src, int maxLineChar);
//no sanitization. sanitize user-inputted strings before passing in.
inline void WriteSaveFile(const char* src, const char* write) {
	FILE* fptr = NULL;
	fptr = fopen(src, "w");
	fprintf(fptr, "%s", write);
	fclose(fptr);
}