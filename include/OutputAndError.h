#pragma once
#include "main.h"
void TokenDump(Vector* vec, Text* text);
void FunctionsAndGlobalVarsDump(Vector* global_vars, Vector* functions);
void PrintErrorInfo(RuntimeError err);
void PrintStrWithUnderline(char* str, char* ofs);
void ErrorUnknownSymbol(Code* code);
void ErrorNotDoubledSign(Code* code, char sign);
void PrintSyntaxError(Text* text, Token* token, SyntaxError err);