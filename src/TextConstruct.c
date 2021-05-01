#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "../include/StackTrace.h"
#include "../include/TextConstruct.h"

enum Error {
  kNoError,
  kNotEnoughMemory
};

size_t LinesCount(char* str) {
  $;
  size_t cnt = 0;
  char* line_end = strchr(str, '\n');
  while (line_end != NULL) {
    ++cnt;
    line_end = strchr(line_end + 1, '\n');
  }

  RETURN cnt + 1;  // cnt + 1 because the end of string is \0, not \n
}

int BufConstruct(Text* text, FILE* file) {
  $;
  assert(text != NULL);
  assert(file != NULL);

  text->buf = (char*)calloc(text->size + 1, sizeof(char));
  if (text->buf == NULL) {
    RETURN kNotEnoughMemory;
  }
  fread(text->buf, sizeof(char), text->size, file);
  text->buf[text->size] = '\0';

  RETURN 0;
}

void LinesConstruct(Text* text) {
  $;
  assert(text != NULL);
  text->lines = (char**)calloc(text->lines_num, sizeof(char*));
  size_t cur_line = 0;
  char* prev_line_end = text->buf - 1;
  char* line_end = strchr(text->buf + 1, '\n');
  while (line_end != NULL) {
    text->lines[cur_line++] = prev_line_end + 1;
    prev_line_end = line_end;
    *line_end = '\0';
    line_end = strchr(line_end + 1, '\n');
  }
  text->lines[cur_line++] = prev_line_end + 1;
  $$;
}

int NewText(Text** text, FILE* file, size_t size) {
  $;
  assert(text != NULL);
  assert(file != NULL);

  *text = (Text*)calloc(1, sizeof(Text));
  (*text)->size = size;

  int err = BufConstruct(*text, file);
  if (err != 0) {
    RETURN err;
  }
  (*text)->lines_num = LinesCount((*text)->buf);
  LinesConstruct(*text);
  
  RETURN 0;
}

void Destroy(Text* text) {
  $;
  free(text->lines);
  free(text->buf);
  free(text);
  $$;
}

// if file contains only 4 symbols like abcd, then size of file is 4 bytes
// if file is like abcd'\n' then size is 5 bytes
size_t SizeOfFile(const char* name) {
  $;
  assert(name != NULL);

  struct stat buf;
  stat(name, &buf);

  RETURN buf.st_size;
}

// int main() {
//   FILE* file = fopen("../prog.neg", "r");
//   Text* text = NULL;
//   int err = NewText(&text, file, SizeOfFile("../prog.neg"));
//   if (err != 0) {
//     printf("error occured %d\n", err);
//   }
//   printf("size = %lu\n", text->size);
//   printf("lines_num = %lu\n", text->lines_num);
//   for (size_t i = 0; i < text->lines_num; ++i) {
//     printf("%s\n", text->lines[i]);
//   }
//   TextDestroy(text);
// }
