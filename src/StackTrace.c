#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/StackTrace.h"

const size_t kCallStackMaxsize = (size_t)1e8;

struct CallStack {
  struct FuncLine* arr;
  size_t sz;
  size_t capacity;
  size_t mincapacity;
  size_t maxcapacity;
  size_t incfac;
} kFuncsCalled = {NULL, 0, 100, 100, 1000, 2};

size_t CallStackPushback(FuncLine func_line);

void FuncsCalledPush(FuncLine func_line) {
  static int cnt = 0;
  if (cnt == 0) {
    kFuncsCalled.arr = (struct FuncLine*)calloc(kFuncsCalled.capacity, sizeof(FuncLine));
    if (kFuncsCalled.arr == NULL) {
      printf("Can not use stack trace, no more memory for stack\n");
    }
  }

  CallStackPushback(func_line);
  ++cnt;
}

FuncLine* FuncsCalledGetValues(size_t* num) {
  if (kFuncsCalled.arr == NULL) {
    return NULL;
  }

  size_t sz = kFuncsCalled.sz;

  if (*num > sz) {
    *num = sz;
  }

  FuncLine* arr = (FuncLine*)calloc(*num, sizeof(int));
  assert(arr != NULL && "Calloc failed in StackTrace.h:FuncsCalledGetValues");

  for (size_t i = 0; i < *num; i++) {
    arr[i] = {(kFuncsCalled.arr[i]).func_name, (kFuncsCalled.arr[i]).line};
  }

  return arr;
}

void CallStackDestroy () {
  if (kFuncsCalled.arr == NULL) {
    return;
  }
  free(kFuncsCalled.arr);
}

void PrintStackInfoAndExit(int signum) {
  static bool called_first_time = true;
  if (!called_first_time) {
    exit(199);
  }
  called_first_time = false;

  switch(signum) {
    case 11:
      printf ("caught segv :^(, error = 11\n");
      break;
    case 6:
      printf ("error = 6\n");
      break;
    default:
      printf("can not recognize signum\n");
      printf("signum: %d\n", signum);
  }

  size_t funcs_in_stack_num = kFuncsCalled.maxcapacity;
  FuncLine* arr = FuncsCalledGetValues(&funcs_in_stack_num);
  if (arr == NULL) {
    printf("0 called functions in stack\n");
    exit(199);
  }

  printf ("%lu called functions in stack:\n", funcs_in_stack_num);
  for (size_t i = 0; i < funcs_in_stack_num; i++) {
    printf("%lu line: %lu, function name: %s\n", i + 1, arr[i].line, arr[i].func_name);
  }

  CallStackDestroy();
  exit(199);
}

size_t CallStackExpand() {
  if (kFuncsCalled.arr == NULL) {
    return 1;
  }

  if (kFuncsCalled.capacity * kFuncsCalled.incfac > kCallStackMaxsize) {
    printf("kCallStackMaxsize reached\n");
    return 1;
  }

  kFuncsCalled.capacity *= kFuncsCalled.incfac;
  kFuncsCalled.arr = (FuncLine*)realloc(kFuncsCalled.arr, kFuncsCalled.capacity * sizeof(FuncLine));

  if (kFuncsCalled.arr == NULL) {
    printf ("No more memory for called functions stack\n");
    return 1;
  }

  return 0;
}

void CallStackShrink() {
  if (kFuncsCalled.arr == NULL) {
    return;
  }

  kFuncsCalled.capacity /= kFuncsCalled.incfac;
  kFuncsCalled.arr = (FuncLine*)realloc(kFuncsCalled.arr, kFuncsCalled.capacity * sizeof(FuncLine));
}

size_t CallStackPushback(FuncLine func_line) {
  if (kFuncsCalled.arr == NULL) {
    return 0;
  }

  if (kFuncsCalled.sz >= kCallStackMaxsize) {
    fprintf(stderr, "call stack maxsize reached\n");
    return 0;
  }

  if (kFuncsCalled.sz == kFuncsCalled.capacity) {
    if (CallStackExpand() != 0) {
      return 0;
    }
  }
  kFuncsCalled.arr[kFuncsCalled.sz++] = func_line;

  return 1;
}

void CallStackPopback () {
  if (kFuncsCalled.sz == 0 || kFuncsCalled.arr == NULL) {
    fprintf(stderr, "CallStackPopback from empty stack!!!\n");
    return;
  }

  kFuncsCalled.arr[--kFuncsCalled.sz] = (struct FuncLine){NULL, 0};
  if (kFuncsCalled.sz > kFuncsCalled.mincapacity &&
      kFuncsCalled.sz <= kFuncsCalled.capacity / (kFuncsCalled.incfac * kFuncsCalled.incfac)) {
    CallStackShrink();
  }
}