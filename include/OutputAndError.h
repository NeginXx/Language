#pragma once
#include "main.h"
void TokenDump(Vector* vec, Text* text);
void VectorFieldsDump(Vector* vec);
void VectorVarDump(Vector* vec, const char* msg);
// void FunctionsAndGlobalVarsDump(Code* code);
void PrintErrorInfo(RuntimeError err);
void PrintStrWithUnderline(char* str, char* ofs);
void ErrorUnknownSymbol(Code* code);
void ErrorNotDoubledSign(Code* code, char sign);
void PrintSyntaxError(Text* text, Token* token, SyntaxError err);