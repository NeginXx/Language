#pragma once
#include <stddef.h>
#include <signal.h>
#define $ FuncsCalledPush((FuncLine){__func__, __LINE__})
#define $$ CallStackPopback()
#define RETURN $$; return
struct FuncLine {
  const char* func_name;
  size_t line;
};
void FuncsCalledPush(FuncLine func_line);
void CallStackPopback();
void PrintStackInfoAndExit(int signum);