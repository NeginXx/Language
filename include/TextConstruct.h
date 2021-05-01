#pragma once
#include <stddef.h>
#include <stdio.h>
struct Text {
  char* buf = NULL;
  char** lines = NULL;
  size_t size = 0;
  size_t lines_num = 0;
};
int NewText(Text** text, FILE* file, size_t size);
void TextDestroy(Text* text);
size_t SizeOfFile(const char* name);