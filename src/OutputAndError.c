#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "../include/StackTrace.h"
#include "../include/Vector.h"
#include "../include/main.h"

void TokenDump(Vector* vec, Text* text) {
	$;
	assert(vec != NULL);
	assert(text != NULL);

	static const char* token_types[] = {  "kUnknownTokenType",
                                        "kElseIf",
                                        "kIf",
                                        "kElse",
                                        "kWhile",
                                        "kBrackO",  
                                        "kBrackC",  
                                        "kParenthO",
                                        "kParenthC",
                                        "kComp",    
                                        "kArithOperator",
                                        "kBoolOperator",
                                        "kReturn",
                                        "kVarName",
                                        "kDecl",
                                        "kEqual",
                                        "kFunc",
                                        "kComma",
                                        "kIntNum",
                                        "kFloatNum",
                                        "kBreak",
                                        "kContinue",
                                        "kNewLine",
                                        "kExit",
                                        "kScan",
                                        "kPrint"      };

	printf("\n--------------------------------\n");
	printf("        arr = %p\n", vec->arr);
	printf("         sz = %lu\n", vec->sz);
	printf("   capacity = %lu\n", vec->capacity);
	printf("mincapacity = %lu\n", vec->mincapacity);
	printf("   increase = %lu\n", vec->increase_capacity);

	printf("\narray contains:\n");
	for (size_t i = 0; i < vec->sz; ++i) {
		Token token = ((Token*)vec->arr)[i];
		if (i != 0) {
			printf("******\n");
		}
		printf("%lu: type = %s\n", i, token_types[token.type]);
		switch (token.type) {
			case kFunc:
				printf("Func = %s\n", token.val.name);
				break;
			case kVar:
				printf("Var = %s\n", token.val.name);
				break;
			case kIntNum:
				printf("int value = %d\n", token.val.int_val);
				break;
			case kFloatNum:
				printf("float value = %f\n", token.val.float_val);
				break;
			default:
				printf("id = %d\n", token.val.id);
				break;
		}
		printf("line = %lu\n", token.pos.line + 1);
		printf("ofs = %s\n", token.pos.ofs);
		printf("str = %s\n\n", text->lines[token.pos.line]);
	}
	printf("--------------------------------\n");
	$$;
}

void VectorFieldsDump(Vector* vec) {
	$;
	assert(vec != NULL);
	printf("        arr = %p\n", vec->arr);
	printf("         sz = %lu\n", vec->sz);
	printf("   capacity = %lu\n", vec->capacity);
	printf("mincapacity = %lu\n", vec->mincapacity);
	printf("   increase = %lu\n\n", vec->increase_capacity);
	$$;
}

void VectorVarDump(Vector* vec, const char* msg) {
	$;
	assert(vec != NULL);
	assert(msg != NULL);

	printf("%s\n", msg);
	VectorFieldsDump(vec);
	for (size_t i = 0; i < vec->sz; ++i) {
		printf("name: %s\n", ((Var*)VectorGet(vec, i, sizeof(Var)))->name);
		printf("type: %d\n\n", ((Var*)VectorGet(vec, i, sizeof(Var)))->type);
	}
}

void FunctionsAndGlobalVarsDump(Code* code) {
	printf("Functions and global vars dump:\n");
	printf("-------------------------\n");
	VectorVarDump(code->global_vars, "global vars");
	Vector* vec = code->functions;
	for (size_t i = 0; i < vec->sz; ++i) {
		if (i > 0 ) {
			printf("*************************\n\n");
		}
		printf("function %lu\n", i + 1);
		Function func = *(Function*)VectorGet(vec, i, sizeof(Function));
		printf("name: %s\n", func.name);
		printf("type: %d\n", func.type);
		printf("args_num: %lu\n", func.args_num);
		printf("args: %p\n", func.args);
		printf("local_vars: %p\n\n", func.local_vars);
		VectorVarDump(func.args, "its args:");
		if (func.local_vars != NULL) {
			VectorVarDump(func.local_vars, "its local vars:");
		}
	}
	printf("-------------------------\n");
}

void PrintErrorInfo(RuntimeError err) {
	switch (err) {
		case kFileExtensionError:
			fprintf(stderr, "error: file format not recognized\n");
			break;
		case kExcessArgsTerminal:
			fprintf(stderr, "error: expected only file name, but excess arguments found\n");
			break;
		case kNoArgsTerminal:
			fprintf(stderr, "error: no arguments in terminal, specify file name\n");
			break;
		case kNotEnoughMemory:
			fprintf(stderr, "error: not enough memory to compile\n");
			break;
		case kNoFileInDir:
			fprintf(stderr, "error: can not open file, probably it's not in the current directory\n");
			break;
		case kEmptyFile:
			fprintf(stderr, "error: file is empty\n");
			break;
		case kUndefinedSymbols:
		case kSyntaxError:
			break;
		default:
			fprintf(stderr, "error: unknown error code %d\n", err);
			break;
	}
}

void PrintStrWithUnderline(char* str, char* ofs) {
	$;
	assert(str != NULL);
	assert(ofs != NULL);

	while (*str == ' ' || *str == '\t') {
		++str;
	}
	fprintf(stderr, "%s\n", str);
	for (; str < ofs; ++str) {
		fprintf(stderr, "%c", *str == '\t' ? '\t' : '-');
	}
	fprintf(stderr, "^");
	$$;
}

void ErrorUnknownSymbol(Code* code) {
	$;
	assert(code != NULL);
	fprintf(stderr, "error: unknown symbol flow on a line %lu\n", code->pos.line + 1);
	PrintStrWithUnderline(code->text->lines[code->pos.line], code->pos.ofs);
	fprintf(stderr, " can not recognize the command\n\n");
}

void ErrorNotDoubledSign(Code* code, char sign) {
	$;
	assert(code != NULL);

	fprintf(stderr, "error: line %lu\n", code->pos.line + 1);
	PrintStrWithUnderline(code->text->lines[code->pos.line], code->pos.ofs);
	fprintf(stderr, "sign %c does not name a command. You meant %c%c?\n", sign, sign, sign);
}

void PrintSyntaxError(Code* code, SyntaxError err) {
	$;
	assert(code != NULL);
}