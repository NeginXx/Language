#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "../include/OutputAndError.h"
#include "../include/StackTrace.h"
#include "../include/Vector.h"
#include "../include/TextConstruct.h"
#include "../include/Tokenize.h"
#include "../include/main.h"

int ER_ = 0;  // this variable is auxiliary and used only in macro
#define CATCH_ERROR(func)   \
  if ((ER_ = func) != 0) {  \
    RETURN ER_;             \
  }

void FreeAllMemory(Vector* not_freed_ptrs) {
	$;
	// void** not_freed_ptr = (void**)not_freed_ptrs->arr;
	// size_t sz = not_freed_ptrs->sz;
	// for (size_t i = 0; i < sz; ++i) {
	// 	printf("fre: %p\n", *not_freed_ptr);
	// 	free(*not_freed_ptr++);
	// }
	// VectorDestroy(not_freed_ptrs);
	$$;
}

void ErrorInvalidFuncDecl(Code* code, Token* token) {
	$;
	assert(code != NULL);

	fprintf(stderr, "error: on a line %lu\n", token->pos.line + 1);
	PrintStrWithUnderline(code->text->lines[token->pos.line], token->pos.ofs);
	fprintf(stderr, " invalid declaration of a function\n");
}

int FuncConstructAddVar(Code* code, Function* func,
size_t* token_idx, bool* is_error_occured) {
	$;
	assert(code != NULL);
	assert(func != NULL);
	assert(token_idx != NULL);
	assert(is_error_occured != NULL);

  Token cur_token = *(Token*)VectorGet(code->tokens, *token_idx, sizeof(Token));
  Var var = {};
  if (cur_token.type == kDecl) {
  	var.type = (VarType)cur_token.val.id;
  } else {
  	*is_error_occured = true;
  	RETURN 0;
  }

  ++*token_idx;
  cur_token = *(Token*)VectorGet(code->tokens, *token_idx, sizeof(Token));
  if (cur_token.type == kVar) {
  	var.name = cur_token.val.name;
  } else {
  	*is_error_occured = true;
  	RETURN 0;
  }

  CATCH_ERROR(VectorPushback(func->args, &var, sizeof(Var)));
  ++func->args_num;

  RETURN 0;
}

int FuncConstruct(Code* code, Function* func,
size_t* token_idx, bool* is_error_occured) {
	$;
	assert(code != NULL);
	assert(func != NULL);
	assert(token_idx != NULL);
	assert(is_error_occured != NULL);

	CATCH_ERROR(NewVector(&func->args, sizeof(Var)));
	CATCH_ERROR(NewVector(&func->local_vars, sizeof(Var)));

	++*token_idx;
	Token cur_token = *(Token*)VectorGet(code->tokens, *token_idx, sizeof(Token));
	if (cur_token.type != kParenthO) {
		*is_error_occured = true;
		RETURN 0;
	}
	++*token_idx;
	cur_token = *(Token*)VectorGet(code->tokens, *token_idx, sizeof(Token));
	if (cur_token.type == kParenthC) {
		RETURN 0;
	}
	bool is_end = false;
	while (!is_end) {
		CATCH_ERROR(FuncConstructAddVar(code, func, token_idx, is_error_occured));
		if (*is_error_occured) {
			RETURN 0;
		}
		++*token_idx;
		cur_token = *(Token*)VectorGet(code->tokens, *token_idx, sizeof(Token));
		if (cur_token.type == kComma) {
			++*token_idx;
		} else if (cur_token.type == kParenthC) {
			is_end = true;
		} else {
			*is_error_occured = true;
			RETURN 0;
		}
	}

	RETURN 0;
}

int ConstructFunctionsAndGlobalVars(Code* code) {
	$;
	assert(code != NULL);

	bool is_error_occured = false;

	Vector* tokens = code->tokens;
	Token prev_token = *(Token*)VectorGet(tokens, 0, sizeof(Token));
	bool is_any_func_met = false;
	for (size_t token_idx = 1; token_idx < tokens->sz; ++token_idx) {
		Token cur_token = *(Token*)VectorGet(tokens, token_idx, sizeof(Token));
		if (!is_any_func_met && cur_token.type == kVar && prev_token.type == kDecl) {

			Var var = {cur_token.val.name, (VarType)prev_token.val.id, 0};
			CATCH_ERROR(VectorPushback(code->global_vars, &var, sizeof(Var)));

		} else if (cur_token.type == kFunc && prev_token.type == kDecl) {

			is_any_func_met = true;
			Function func = {cur_token.val.name, (FuncType)prev_token.val.id, 0, NULL, NULL};
		  CATCH_ERROR(FuncConstruct(code, &func, &token_idx, &is_error_occured));
		  if (is_error_occured) {
		  	ErrorInvalidFuncDecl(code, &cur_token);
		  	cur_token.type = kNewLine;
		  	VectorDestroy(func.args);
		  	VectorDestroy(func.local_vars);
		  } else {
		  	CATCH_ERROR(VectorPushback(code->functions, &func, sizeof(Function)));
		  }

		}
		prev_token = cur_token;
	}

	RETURN is_error_occured ? kFuncDeclError : 0;
}

int NewNode(Node** node) {
	$;
	*node = (Node*)calloc(1, sizeof(Node));
	if (*node == NULL) {
		RETURN kNotEnoughMemory;
	}

	CATCH_ERROR( NewVector(&((*node)->children), sizeof(Node*)) );
	RETURN 0;
}

inline Token* GetToken(Vector* vec, size_t idx) {
	return (Token*)VectorGet(vec, idx, sizeof(Token));
}

struct CreateNodeArgs {
	Node* node = NULL;
	Code* code = NULL;
	size_t* token_idx = NULL;
	bool* is_error_occured = NULL;
};

// if error occured NULL node is returned
namespace CreateNode {
	void AssertArgs(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->code != NULL);
		assert(args->token_idx != NULL);
		assert(args->is_error_occured = NULL);
		$$;
	}

	int VarDeclaration(CreateNodeArgs* args) {
		$;
		AssertArgs(args);
		size_t* idx = args->token_idx;
		Code* code = args->code;

		Token* cur_token = GetToken(code->tokens, *idx);
		Node* node = NULL;
		CATCH_ERROR(NewNode(&node));
		Token* cur_token = (Token*);
	}

	int Declaration(CreateNodeArgs* args) {
		$;
		AssertArgs(args);

		Node* node = NULL;
		CATCH_ERROR(NewNode(&node));
		Token* cur_token = (Token*)VectorGet(args->code->tokens, *token_idx, sizeof(Token));
		if (cur_token->type == kVar) {
			node->token = cur_token;

		}
	}
}

void TokenAdvTillNewLine(Code* code, size_t* token_idx) {
	$;
	assert(code != NULL);
	assert(token_idx != NULL);

	Token* cur_token = (Token*)VectorGet(code->tokens, *token_idx, sizeof(Token));
	while (cur_token.type != kNewLine && *token_idx < code->tokens->sz) {
		++*token_idx;
		cur_token = *(Token*)VectorGet(code->tokens, *token_idx, sizeof(Token));
	}
	++*token_idx;
	$$;
}

int TreeConstruct(Code* code, Node* root) {
	$;
	assert(code != NULL);
	assert(root != NULL);

	bool syn_err_occured = false;
	CATCH_ERROR(NewVector(&root->children, sizeof(Node*)));
	size_t token_idx = 0;
	CreateNodeArgs args = {};
	Token cur_token = {};

	while (token_idx < code->tokens->sz) {
		args = {};
		cur_token = *(Token*)VectorGet(code->tokens, token_idx, sizeof(Token));
		Node* node = NULL;
		switch (cur_token.type) {
			case kDecl:
				++token_idx;
			  args = {&node, code, &token_idx, &is_error_occured};
				CATCH_ERROR(CreateNode::Declaration(args));
				break;
			default:
				is_error_occured = true;
				PrintSyntaxError(kFuncDeclError);
				TokenAdvTillNewLine(code, &token_idx);
				break;
		}
		if (!is_error_occured) {
			cur_token = *(Token*)VectorGet(code->tokens, token_idx, sizeof(Token));
			if (cur_token.type != kNewLine) {
				is_error_occured = true;
				PrintSyntaxError(kNewLineError);
				TokenAdvTillNewLine(code, &token_idx);
			}

			if (!is_error_occured) {
				node->parent = root;
				CATCH_ERROR(VectorPushback(root->children, node, sizeof(Node*)));
			}
		}

		if (is_error_occured) {
			syn_err_occured = true;
		}
	}

	RETURN syn_err_occured ? kSyntaxError : 0;
}

#define DEBUG

int main(int argc, const char** argv) {
	$;
	signal(SIGSEGV, PrintStackInfoAndExit);
	signal(SIGABRT, PrintStackInfoAndExit);

	#ifdef DEBUG
		argc = 2;
		const char* argvv[2] = {"./file", "prog.neg"};
		argv = argvv;
	#endif

	int err = 0;
	#define CHECK_FOR_ERROR(func)       \
		if ((err = func) != 0) {          \
			PrintErrorInfo(err);            \
			FreeAllMemory(&not_freed_ptrs); \
			RETURN err;                     \
		}

	Vector not_freed_ptrs = {};
	CHECK_FOR_ERROR(VectorConstruct(&not_freed_ptrs, sizeof(void*)));
	Code code = {};
  CHECK_FOR_ERROR(CodeConstruct(&code, argc, argv, &not_freed_ptrs));
	CHECK_FOR_ERROR(Tokenize(&code));

	// CHECK_FOR_ERROR(ConstructFunctionsAndGlobalVars(&code));
	// TokenDump(code.tokens, code.text);
	// FunctionsAndGlobalVarsDump(&code);
	// FreeAllMemory(&not_freed_ptrs);

	Node root = {};
	CHECK_FOR_ERROR(TreeConstruct(&code, &root));

	printf("compilation finished successfully\n");
	RETURN 0;
}