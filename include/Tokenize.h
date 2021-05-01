#pragma once
#include "main.h"
int CodeConstruct(Code* code, int argc, const char** argv,
Vector* not_freed_ptrs);
int Tokenize(Code* code);