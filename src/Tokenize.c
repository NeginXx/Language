#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "../include/OutputAndError.h"
#include "../include/StackTrace.h"
#include "../include/TextConstruct.h"
#include "../include/Vector.h"
#include "../include/main.h"

static int ER_ = 0;  // this variable is auxiliary and used only in macro
#define CATCH_ERROR(func)   \
  if ((ER_ = func) != 0) {  \
    RETURN ER_;             \
  }

void OfsAdv(Code* code) {
	$;
	++code->pos.ofs;
  while (*code->pos.ofs == ' ' || *code->pos.ofs == '\t') {
  	++code->pos.ofs;
  }
  $$;
}

int CheckTerminalArgsCorrectness(int argc, const char** argv) {
	$;
	assert(argc >= 0);
	assert(argv != NULL);

	if (argc > 2) {
		RETURN kExcessArgsTerminal;
	} else if (argc < 2) {
		RETURN kNoArgsTerminal;
	} else {
		const char* dot = strchr(argv[1], '.');
		if (dot == NULL || strcmp(dot, kFileExtension) != 0) {
			RETURN kFileExtensionError;
		}
	}

	RETURN 0;
}

int CodeConstructBufferFields(Code* code, int argc, const char** argv,
Vector* not_freed_ptrs) {
	$;
	assert(argc != 0);
	assert(code != NULL);
	assert(argv != NULL);

	CATCH_ERROR(CheckTerminalArgsCorrectness(argc, argv));
	FILE* file = fopen(argv[1], "r");
	if (file == NULL) {
		RETURN kNoFileInDir;
	}

	size_t sz = SizeOfFile(argv[1]);
	if (sz == 0) {
		fclose(file);
		RETURN kEmptyFile;
	}

	int err = NewText(&code->text, file, sz);
	fclose(file);
	if (err != 0) {
		RETURN err;
	}
	CATCH_ERROR(VectorPushback(not_freed_ptrs, &code->text->buf, sizeof(code->text->buf)));
	CATCH_ERROR(VectorPushback(not_freed_ptrs, &code->text->lines, sizeof(code->text->lines)));
	CATCH_ERROR(VectorPushback(not_freed_ptrs, &code->text, sizeof(code->text)));
	code->pos.ofs = code->text->lines[0];
	code->pos.line = 0;

	CATCH_ERROR(NewNamesBuf(&code->names_buf));
	CATCH_ERROR(VectorPushback(not_freed_ptrs, &code->names_buf->buf, sizeof(code->names_buf->buf)));
	CATCH_ERROR(VectorPushback(not_freed_ptrs, &code->names_buf, sizeof(code->names_buf)));

	RETURN 0;
}

int CodeConstructVector(Vector** code_vec_ptr,
Vector* not_freed_ptrs, size_t type_sz) {
	$;
	CATCH_ERROR(NewVector(code_vec_ptr, type_sz));
	CATCH_ERROR(VectorPushback(not_freed_ptrs, &((*code_vec_ptr)->arr), sizeof((*code_vec_ptr)->arr)));
	CATCH_ERROR(VectorPushback(not_freed_ptrs, code_vec_ptr, sizeof(*code_vec_ptr)));
	RETURN 0;
}

int CodeConstruct(Code* code, int argc, const char** argv,
Vector* not_freed_ptrs) {
	$;
	CATCH_ERROR(CodeConstructBufferFields(code, argc, argv,  not_freed_ptrs));
	CATCH_ERROR(CodeConstructVector(&code->tokens, not_freed_ptrs, sizeof(Token)));
	RETURN 0;
}

bool IsSymbolDelimiter(char c) {
	$;
	char delim_syms[] = { '(', ')', '[', ']', ',', '=', '<',
	                      '>', '!', '&', '|', '+', '-', '*',
	                      '/', '\0', '\n', '\t', ' ' };
	size_t sz = sizeof(delim_syms)/sizeof(delim_syms[0]);
	for (size_t i = 0; i < sz; ++i) {
		if (c == delim_syms[i]) {
			RETURN true;
		}
	}
	RETURN false;
}

bool IsNum(Code* code, Token* token) {
	$;
	assert(code != NULL);
	assert(token != NULL);

	char* ofs = code->pos.ofs;
	char* end_of_num = NULL;
	float val = strtof(ofs, &end_of_num);
	if (end_of_num == ofs) {
		RETURN false;
	} else if (!IsSymbolDelimiter(end_of_num[0])) {
		ErrorUnknownSymbol(code);
		RETURN true;
	}

	char* dot = (char*)memchr(ofs, '.', end_of_num - ofs);
	if (dot != NULL) {
		token->type = kFloatNum;
		token->val.float_val = val;
		code->pos.ofs = end_of_num - 1;
		RETURN true;
	}

	token->type = kIntNum;
	token->val.int_val = (int)(val + 0.1); // to avoid floating point inprecision
	code->pos.ofs = end_of_num - 1;
	RETURN true;
}

struct Keyword {
	const char* name = NULL;
	TokenType type = (TokenType)0;
	int token_val_id = 0;
	size_t name_len = 0;
};

bool IsKeyword(Code* code, Token* token) {
	$;
	assert(code != NULL);
	assert(token != NULL);

	static Keyword keywords[] = {
		{"int", kDecl, kDeclInt, 3},
		{"float", kDecl, kDeclFloat, 5},
		{"void", kDecl, kDeclVoid, 4},
		{"return", kReturn, 0, 6},
		{"while", kWhile, 0, 5},
		{"if", kIf, 0, 2},
		{"else if", kElseIf, 0, 7},
		{"else", kElse, 0, 4},
		{"break", kBreak, 0, 5},
		{"continue", kContinue, 0, 8},
		// {"exit", kExit, 0, 4},
		// {"scan", kScan, 0, 4},
		// {"print", kPrint, 0, 5}
	};

	size_t sz = sizeof(keywords) / sizeof(keywords[0]);
	char* ofs = code->pos.ofs;
	for (size_t i = 0; i < sz; ++i) {
		char replaced_char = *(ofs + keywords[i].name_len);
		*(ofs + keywords[i].name_len) = '\0';
		if (strcmp(ofs, keywords[i].name) == 0) {
			char last_symbol = replaced_char;
			if (IsSymbolDelimiter(replaced_char)) {
				code->pos.ofs += keywords[i].name_len - 1;
				token->type = keywords[i].type;
				token->val.id = keywords[i].token_val_id;
				*(ofs + keywords[i].name_len) = replaced_char;
				RETURN true;
			}
		}
		*(ofs + keywords[i].name_len) = replaced_char;
	}

	RETURN false;
}

bool IsNextSymbolParantheses(char* ofs) {
	$;
	assert(ofs != NULL);

	while (*ofs == ' ' || *ofs == '\t') {
		++ofs;
	}
	if (*ofs == '(') {
		RETURN true;
	}
	RETURN false;
}

bool IsName(Code* code, Token* token) {
	$;
	assert(code != NULL);
	assert(token != NULL);

  char* ofs = code->pos.ofs;
  if (!isalnum(*ofs) && *ofs != '_') {
  	RETURN false;
  }
  while (isalnum(*ofs) || *ofs == '_') {
  	++ofs;
  }
  while (isdigit(*ofs)) {
  	++ofs;
  }
  CATCH_ERROR(NamesBufPushback(code->names_buf, code->pos.ofs, ofs - code->pos.ofs));
 	if (IsNextSymbolParantheses(ofs)) {
 		token->type = kFunc;
 	} else {
 		token->type = kVar;
 	}
 	token->val.name = code->names_buf->last_str;
 	code->pos.ofs = ofs - 1;
 	RETURN true;
}

void TokenizeCaseDefault(Code* code, Token* token) {
	$;
	assert(code != NULL && "TokenizeCaseDefault assertion failed");
	assert(token != NULL && "TokenizeCaseDefault assertion failed");

	if (IsNum(code, token)) {
		RETURN;
	}

	if (IsKeyword(code, token)) {
		RETURN;
	}

	if (IsName(code, token)) {
		RETURN;
	}

	ErrorUnknownSymbol(code);
  $$;
}

int CheckIfFileEmptyAndOfsAdv(Code* code) {
	$;
	assert(code != NULL);

	char* ofs = NULL;
	for (size_t i = 0; i < code->text->lines_num; ++i) {
		ofs = code->text->lines[i];
	  while (*ofs == ' ' || *ofs == '\t') {
	  	++ofs;
	  }
	  if (*ofs != '\0') {
	  	RETURN kNoError;
	  }
	}

	RETURN kEmptyFile;
}

int TokenizeOneLine(Code* code, bool* is_error_occured) {
	$;
	assert(code != NULL);
	assert(is_error_occured != NULL);

	Token token = {};
	char** ofs = &code->pos.ofs;
	if (**ofs == ' ' || **ofs == '\t') {
		OfsAdv(code);
	}

	bool is_end_of_line = false;
	while (!is_end_of_line) {
		token = {kUnknownTokenType, 0, code->pos};
		switch (**ofs) {
			case '(':
				token.type = kParenthO;
				break;
			case ')':
				token.type = kParenthC;
				break;
			case '[':
				token.type = kBrackO;
				break;
			case ']':
				token.type = kBrackC;
				break;

			case ',':
				token.type = kComma;
				break;

			case '=':
				if (*(*ofs + 1) == '=') {
					++*ofs;
					token.type = kComp;
					token.val.id = kEqualEqual;
				} else {
					token.type = kEqual;
				}
				break;

			case '<':
				if (*(*ofs + 1) == '=') {
					++*ofs;
					token.type = kComp;
					token.val.id = kLessEqual;
				} else {
					token.type = kComp;
					token.val.id = kLess;
				}
				break;

			case '>':
				if (*(*ofs + 1) == '=') {
					++*ofs;
					token.type = kComp;
					token.val.id = kGreaterEqual;
				} else {
					token.type = kComp;
					token.val.id = kGreater;
				}
				break;

			case '!':
				if (*(*ofs + 1) == '=') {
					++*ofs;
					token.type = kComp;
					token.val.id = kGreaterEqual;
				}
				break;

			case '&':
				if (*(*ofs + 1) == '&') {
					++*ofs;
					token.type = kBoolOperator;
					token.val.id = kAnd;
				} else {
					ErrorNotDoubledSign(code, '&');
					token.type = kUnknownTokenType;
				}
				break;

			case '|':
				if (*(*ofs + 1) == '|') {
					++*ofs;
					token.type = kBoolOperator;
					token.val.id = kOr;
				} else {
					ErrorNotDoubledSign(code, '|');
					token.type = kUnknownTokenType;
				}
				break;

			case '+':
				token.type = kArithOperator;
				token.val.id = kPlus;
				break;
			case '-':
				token.type = kArithOperator;
				token.val.id = kMinus;
				break;
			case '*':
				token.type = kArithOperator;
				token.val.id = kMul;
				break;
			case '/':
				if (*(*ofs + 1) == '/') {  // commnent ecnountered
					token.type = kNewLine;
				} else {
					token.type = kArithOperator;
					token.val.id = kDiv;
				}
				break;

			case '\0':
				token.type = kNewLine;
				break;

			default:
				TokenizeCaseDefault(code, &token);
				break;
		}

		if (token.type == kNewLine) {
			is_end_of_line = true;
			if (((Token*)code->tokens->arr)[code->tokens->sz - 1].type != kNewLine) {
				CATCH_ERROR(VectorPushback(code->tokens, &token, sizeof(token)));
			}
		} else if (token.type != kUnknownTokenType) {
			OfsAdv(code);
			CATCH_ERROR(VectorPushback(code->tokens, &token, sizeof(token)));
		} else {
			*is_error_occured = true;
			is_end_of_line = true;
		}
	}
	
	RETURN 0;
}

int Tokenize(Code* code) {
	$;
	assert(code != NULL);

	CATCH_ERROR(CheckIfFileEmptyAndOfsAdv(code));
	bool is_error_occured = false;
	for (size_t cur_line = 0; cur_line < code->text->lines_num; ++cur_line) {
		code->pos.ofs = code->text->lines[cur_line];
		code->pos.line = cur_line;
		CATCH_ERROR(TokenizeOneLine(code, &is_error_occured));
	}

	RETURN is_error_occured ? kUndefinedSymbols : 0;
}