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

void FunctionsAndGlobalVarsDump(Vector* global_vars, Vector* functions) {
	assert(global_vars != NULL);
	assert(functions != NULL);
	printf("Functions and global vars dump:\n");
	printf("-------------------------\n");
	VectorVarDump(global_vars, "global vars");
	Vector* vec = functions;
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
			fprintf(stderr, "error: file format not recognized\nShould be .neg\n");
			break;
		case kExcessArgsTerminal:
			fprintf(stderr, "error: too many arguments in command line\nType -h for help\n");
			break;
		case kNoArgsTerminal:
			fprintf(stderr, "error: no arguments in terminal\nNeed help? Specify -h\n");
			break;
		case kNoInputFileFlag:
			fprintf(stderr, "error: compilation flags were specified, no file name tho\nTry -h for help\n");
			break;
		case kWrongFileFlagFormat:
			fprintf(stderr, "error: -o found, no file specified tho\n");
			break;
		case kNoCompileFlag:
			fprintf(stderr, "error: file name is specified, no compilation flags tho\n");
			break;
		case kNotEnoughMemory:
			fprintf(stderr, "error: not enough memory to compile\n");
			break;
		case kNoIolibFile:
			fprintf(stderr, "error: couldn't find iolib.asm file in directory\n");
			break;
		case kNoFileInDir:
			fprintf(stderr, "error: can not open file, probably it's not in the current directory\n");
			break;
		case kEmptyFile:
			fprintf(stderr, "error: file is empty\n");
			break;
		case kUndefinedSymbols:
		case kSyntaxError:
		case kCompilationError:
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
	fprintf(stderr, "^ ");
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

void PrintSyntaxError(Text* text, Token* token, SyntaxError err) {
	$;
	assert(text != NULL);
	assert(token != NULL);

	Position pos = token->pos;
	fprintf(stderr, "error: syntax error on line %lu\n", pos.line + 1);
	PrintStrWithUnderline(text->lines[token->pos.line], token->pos.ofs);
	switch (err) {
		case kFuncVarDeclExpectedError:
			fprintf(stderr, "expected function or global variable declaration\n\n");
			break;
		case kFuncVarDeclErrorNameExpected:
			fprintf(stderr, "expected function or variable name\n\n");
			break;
		case kEqualAfterVarExpectedError:
			fprintf(stderr, "expected equal sign after variable name\n\n");
			break;
		case kVarDeclError:
			fprintf(stderr, "invalid variable declaration\n\n");
			break;
		case kStatementNewLineExpectedError:
			fprintf(stderr, "expected new line at end of statement\n\n");
			break;
		case kVarDeclNewLineExpectedError:
			fprintf(stderr, "expected new line at end of variable declaration\n\n");
			break;
		case kCommaParenthExpectedError:
			fprintf(stderr, "expected comma or closing parenthesis )\n\n");
			break;
		case kVarDeclExpectedError:
			fprintf(stderr, "expected declarator int or float\n\n");
			break;
		case kVarNameExpectedError:
			fprintf(stderr, "expected variable name\n\n");
			break;
		case kBracketOpenExpectedError:
			fprintf(stderr, "expected open bracket [\n\n");
			break;
		case kBracketCloseExpectedError:
			fprintf(stderr, "expected closing bracket ]\n\n");
			break;
		case kParenthOpenExpectedError:
			fprintf(stderr, "expected open parenthesis (\n\n");
			break;
		case kParenthCloseExpectedError:
			fprintf(stderr, "expected closing parenthesis )\n\n");
			break;
		case kStatementExpectedError:
			fprintf(stderr, "expected statement\n\n");
			break;
		case kExpressionExpectedError:
			fprintf(stderr, "expected expression\n\n");
			break;
		case kCompSignExpectedError:
			fprintf(stderr, "expected compare sign\n\n");
			break;
		case kUnexpectedSymbolError:
			fprintf(stderr, "unexpected symbol in expression\n\n");
			break;
		default:
			fprintf(stderr, "can not recognize error code\n\n");
	}
}

void PrintCompilationError(Text* text, Token* token, CompilationError err) {
	$;
	assert(text != NULL);
	assert(token != NULL);

	Position pos = token->pos;
	fprintf(stderr, "error: compilation error on line %lu\n", pos.line + 1);
	PrintStrWithUnderline(text->lines[token->pos.line], token->pos.ofs);
	switch (err) {
		case kNoReturnError:
			fprintf(stderr, "nonvoid function with no return\n\n");
			break;
		case kVoidReturnError:
		  fprintf(stderr, "empty return in nonvoid function\n\n");
		  break;
		case kNonVoidReturnError:
			fprintf(stderr, "return of expression in void function\n\n");
			break;
		case kInvalidReturnTypeError:
			fprintf(stderr, "return expression type does not match function type\n\n");
			break;
		case kVarInitInvalidTypeError:
			fprintf(stderr, "variable and expression have different types\n\n");
			break;
		case kNoSuchVarError:
			fprintf(stderr, "reference to undeclared variable\n\n");
			break;
		case kNoSuchFuncError:
			fprintf(stderr, "reference to undeclared function\n\n");
			break;
		case kInvalidVarTypeError:
			fprintf(stderr, "invalid variable declaration, variables can only be int or float\n\n");
			break;
		case kDifferentTypeOperandsError:
			fprintf(stderr, "operands to binary operation have different types\n\n");
			break;
		case kVoidFuncInExpr:
			fprintf(stderr, "void function used in expression\n\n");
			break;
		case kIncorrectArgumentsNumToFunc:
			fprintf(stderr, "number of function arguments doesn't match with declaration\n\n");
			break;
		case kIncorrecArgumentType:
			fprintf(stderr, "function argument type doesn't match with declaration\n\n");
			break;
		case kVarRedeclaration:
			fprintf(stderr, "redeclaration of variable\n\n");
			break;
		default:
			fprintf(stderr, "can not recognize error code\n\n");
	}
}