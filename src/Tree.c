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

  void NodeDump(Node* node);

namespace Dot {
	const char* kRGBColorBlack = "#000000";
	const char* kRGBColorWhite       = "#FFFFFF";
	const char* kRGBColorDarkskyblue = "#4B738A";
	const char* kRGBColorSkyblue     = "#00C4FF";
	const char* kRGBColorBlue        = "#006FFF";
	const char* kRGBColorRed         = "#FF0000";
	const char* kRGBColorGreen       = "#16BA37";
	const char* kRGBColorOlive       = "#A69B00";
	const char* kRGBColorGrey        = "#616161";
	const char* kRGBColorPurple      = "#B300FF";
	const char* kRGBColorDarkpurple  = "#480066";
	const char* kRGBColorPink        = "#FF00C4";

	void NodeDeclare(Node* node, FILE* file) {
		$;
		assert(file != NULL);
		const char* comp_signs[] = {"less", "less equal", "greater", "greater equal", "==", "!="};
		const char* arith_operators[] = {"+", "-", "*", "/"};
		const char* bool_operators[] = {"&&", "||", "not !"};
		const char* decl_types[] = {"int", "float", "void"};
		static const char* token_types[] = {"unknown",
                                        "elif",
                                        "if",
                                        "else",
                                        "while",
                                        "[",  
                                        "]",  
                                        "(",
                                        ")",
                                        "comp sign",    
                                        "arith op",
                                        "bool op",
                                        "return",
                                        "var",
                                        "declaration",
                                        "=",
                                        "func",
                                        ",",
                                        "int num",
                                        "float num",
                                        "break",
                                        "continue",
                                        "new line",
                                        "exit",
                                        "scan",
                                        "print"      };

		static char label[50] = {};
		if (node != NULL && node->token != NULL) {
			char value[50] = {};
			switch (node->token->type) {
				case kComp:
					sprintf(value, " | %s", comp_signs[node->token->val.id]);
					break;
				case kArithOperator:
					sprintf(value, " | %s", arith_operators[node->token->val.id]);
					break;
				case kBoolOperator:
					sprintf(value, " | %s", bool_operators[node->token->val.id]);
					break;
			  case kVar:
				case kFunc:
					sprintf(value, " | %s | %s", decl_types[(node->token - 1)->val.id], node->token->val.name);
					break;
				case kDecl:
				  sprintf(value, " | %s", decl_types[node->token->val.id]);
					break;
				case kIntNum:
					sprintf(value, " | %d", node->token->val.int_val);
					break;
				case kFloatNum:
					sprintf(value, " | %f", node->token->val.float_val);
					break;
				case kBrackO:
					sprintf(value, " | StatementBody");
					break;
			}
			sprintf(label, "%s%s", token_types[node->token->type], value);
		} else if (node != NULL && node->token == NULL) {
			sprintf(label, "root");
		} else {
			sprintf(label, "NULL");
		}

		fprintf(file, "\t\"%p\" [fillcolor = \"%s\", label = \"%s\"]\n", node, kRGBColorPurple, label);
		$$;
	}

	void NodeDump(Node* node, FILE* file) {
		$;
		assert(file != NULL);

		// if (node->parent->left == node) {
		// 	fprintf(file, "\"%p\":sw->\"%p\"\n", node->parent, node);
		// 	fprintf(file, "\"%p\"->\"%p\"\n", node, node->parent);
		// } else if (node->parent->right == node) {
		// 	fprintf(file, "\"%p\":se->\"%p\"\n", node->parent, node);
		// 	fprintf(file, "\"%p\"->\"%p\"\n", node, node->parent);
		// }
	  fprintf(file, "\"%p\"->\"%p\"\n", node->parent, node);
		// fprintf(file, "\"%p\"->\"%p\"\n", node, node->parent);
		$$;
	}

	void SubtreeDump(Node* node, FILE* file) {
		$;
		assert(file != NULL);

		NodeDeclare(node, file);
		if (node == NULL) {
			RETURN;
		}

		for (size_t i = 0; i < node->children->sz; ++i) {
			Node* child = *(Node**)VectorGet(node->children, i, sizeof(Node*));
			SubtreeDump(child, file);
		}
		NodeDump(node, file);
		$$;
	}

	void TreeDump(Node* root) {
		$;
		assert(root != NULL);

		FILE* file = fopen("dump", "w");
		fprintf(file, "digraph G {\n");
		fprintf(file, "node [shape = \"record\","
			            " style = \"filled\", fillcolor = "
			            "\"%s\"]\n", kRGBColorBlue);
		SubtreeDump(root, file);
		fprintf(file, "}\n");

		fclose(file);

		system("dot -Tpng -O dump\n");
	  system("xdg-open dump.png");
	  $$;
	}
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

int NewNode(Node** node) {
	$;
	assert(node != NULL);
	assert(*node == NULL);

	*node = (Node*)calloc(1, sizeof(Node));
	if (*node == NULL) {
		RETURN kNotEnoughMemory;
	}

	CATCH_ERROR( NewVector(&((*node)->children), sizeof(Node*)) );
	RETURN 0;
}

Token* GetToken(TreeInfo* tree) {
	$;
	assert(tree != NULL);
	Token* token = (Token*)VectorGet(tree->tokens, tree->token_idx, sizeof(Token));
	++tree->token_idx;
	RETURN token;
}

Token* GetTokenWithoutAdvancing(TreeInfo* tree) {
	$;
	assert(tree != NULL);
	Token* token = (Token*)VectorGet(tree->tokens, tree->token_idx, sizeof(Token));
	RETURN token;
}

Token* GetPreviousToken(TreeInfo* tree) {
	$;
	assert(tree != NULL);
	RETURN (Token*)VectorGet(tree->tokens, tree->token_idx - 1, sizeof(Token));
}

void GetTokenStepBack(TreeInfo* tree) {
	$;
	assert(tree != NULL);
	--tree->token_idx;
	$$;
}

void TokenAdvOnNewLine(TreeInfo* tree) {
	$;
	assert(tree != NULL);
	Token* token = GetToken(tree);
	while (token->type != kNewLine && tree->token_idx < tree->tokens->sz) {
		token = GetToken(tree);
	}
	$$;
}

void SkipNewLineIfAny(TreeInfo* tree) {
	$;
	assert(tree != NULL);
	if (GetTokenWithoutAdvancing(tree)->type == kNewLine) {
		++tree->token_idx;
	}
	$$;
}

int NodePushback(Vector* vec, Node** node_ptr) {
	return VectorPushback(vec, node_ptr, sizeof(Node*));
}

int AddChildren(Node* destination, Node** node_to_add) {
	return VectorPushback(destination->children, node_to_add, sizeof(Node*));
}

void FindNearestOpenBracket(TreeInfo* tree, bool* is_open_brack_found_near) {
	$;
	assert(tree != NULL);
	assert(is_open_brack_found_near != NULL);
	while (true) {
		assert(tree->token_idx < tree->tokens->sz && "This error is weird, probably file was damaged");
		Token* token = GetToken(tree);
		if (token->type == kBrackO) {
			*is_open_brack_found_near = true;
			GetTokenStepBack(tree);
			RETURN;
		} else if (token->type == kNewLine) {
			token = GetToken(tree);
			if (token->type == kBrackO) {
				*is_open_brack_found_near = true;
				GetTokenStepBack(tree);
				RETURN;
			} else {
				RETURN;
			}
		}
	}
}

int ProcessErrorSkippingStatementBodyIfAny(TreeInfo* tree);
void ProcessErrorSkippingToNewLineOrCloseBracket(TreeInfo* tree);

struct CreateNodeArgs {
	TreeInfo* tree = NULL;
	Node* node = NULL;
	bool is_error_occured = false;
};

// if error occured NULL node is returned
namespace CreateNode {
	int Statement(CreateNodeArgs* args);

	void NotifyAboutError(CreateNodeArgs* args, Token* token, SyntaxError err) {
		$;
	  args->is_error_occured = true;
		PrintSyntaxError(args->tree->text, token, err);
		$$;
	}

	void SetParent(CreateNodeArgs* new_args, CreateNodeArgs* old_args) {
		$;
		if (new_args->node != NULL) {
			new_args->node->parent = old_args->node;
		}
		$$;
	}

	void ProcessCreatedNode(CreateNodeArgs* new_args, CreateNodeArgs* old_args) {
		$;
		AddChildren(old_args->node, &new_args->node);
		SetParent(new_args, old_args);
		$$;
	}

	CreateNodeArgs MakeNewArgs(CreateNodeArgs* old_args) {
		$;
		assert(old_args != NULL);
		CreateNodeArgs args = {};
		args.tree = old_args->tree;
		RETURN args;
	}

	int Expr(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		Token* token = GetToken(args->tree);
		if (token->type == kIntNum || token->type == kFloatNum) {
			CATCH_ERROR(NewNode(&args->node));
			args->node->token = token;
		} else {
			NotifyAboutError(args, token, kShitt);
		}
		RETURN 0;
	}

	int Inequality(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		CATCH_ERROR(NewNode(&args->node));

		CreateNodeArgs new_args = MakeNewArgs(args);
		CATCH_ERROR(CreateNode::Expr(&new_args));
		if (new_args.is_error_occured) {
			RETURN 0;
		}
		ProcessCreatedNode(&new_args, args);

		Token* token = GetToken(args->tree);
		if (token->type != kComp) {
			NotifyAboutError(args, token, kCompSignExpectedError);
			RETURN 0;
		}
		args->node->token = token;

		new_args = MakeNewArgs(args);
		CATCH_ERROR(CreateNode::Expr(&new_args));
		if (new_args.is_error_occured) {
			RETURN 0;
		}
		ProcessCreatedNode(&new_args, args);

		RETURN 0;
	}

	int Condition(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		Token* token = GetToken(args->tree);
		if (token->type != kParenthO) {
			NotifyAboutError(args, token, kBracketOpenExpectedError);
			RETURN 0;
		}

		CreateNodeArgs new_args = MakeNewArgs(args);
		CATCH_ERROR(CreateNode::Inequality(&new_args));
		if (!new_args.is_error_occured) {
			ProcessCreatedNode(&new_args, args);
		} else {
			RETURN 0;
		}

	  token = GetToken(args->tree);
		if (token->type != kParenthC) {
			NotifyAboutError(args, token, kBracketCloseExpectedError);
		}
		RETURN 0;
	}

	int StatementBody(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		Token* token = GetToken(args->tree);
		if (token->type != kBrackO) {
			NotifyAboutError(args, token, kBracketOpenExpectedError);
			RETURN 0;
		}

		SkipNewLineIfAny(args->tree);
		CreateNodeArgs new_args = MakeNewArgs(args);
		bool is_end = false;
		while (!is_end) {
			token = GetTokenWithoutAdvancing(args->tree);
			if (token->type == kBrackC) {
				++args->tree->token_idx;
				is_end = true;
			} else {
				new_args.node = NULL;
				CATCH_ERROR(CreateNode::Statement(&new_args));
				if (!new_args.is_error_occured) {
					ProcessCreatedNode(&new_args, args);
				} else {
					ProcessErrorSkippingToNewLineOrCloseBracket(args->tree);
				}
			}
			SkipNewLineIfAny(args->tree);
		}

		RETURN 0;
	}

  // Loop | If | VarDecl '\n' | FuncCall '\n' | 'return'
	int Statement(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		CATCH_ERROR(NewNode(&args->node));
		Token* token = GetToken(args->tree);
		args->node->token = token;

		CreateNodeArgs new_args = {};
		switch (token->type) {
			case kWhile:
			case kElseIf:
			case kIf:
				CATCH_ERROR(CreateNode::Condition(args));
				if (new_args.is_error_occured) {
					ProcessErrorSkippingStatementBodyIfAny(args->tree);
					RETURN 0;
				}
				CATCH_ERROR(CreateNode::StatementBody(args));
				if (new_args.is_error_occured) {
					RETURN 0;
				}
				break;

			case kReturn:
				token = GetTokenWithoutAdvancing(args->tree);
				if (token->type != kNewLine && token->type != kBrackC) {
					new_args = MakeNewArgs(args);
					CATCH_ERROR(CreateNode::Expr(&new_args));
					if (!new_args.is_error_occured) {
						ProcessCreatedNode(&new_args, args);
					} else {
						ProcessErrorSkippingToNewLineOrCloseBracket(args->tree);
						RETURN 0;
					}
					token = GetTokenWithoutAdvancing(args->tree);
					if (token->type != kNewLine && token->type != kBrackC) {
						NotifyAboutError(args, token, kStatementNewLineExpectedError);
					}
				} else if (token->type == kNewLine) {
					++args->tree->token_idx;
				}
				break;

			default:
				NotifyAboutError(args, token, kShit);
		}
		RETURN 0;
	}

	int VarDeclarationCaseEqual(CreateNodeArgs* args, bool is_global_var) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		CreateNodeArgs new_args = MakeNewArgs(args);
		CATCH_ERROR(CreateNode::Expr(&new_args));
		ProcessCreatedNode(&new_args, args);

		if (new_args.is_error_occured) {
			TokenAdvOnNewLine(args->tree);
			RETURN 0;
		}
		Token* token = GetToken(args->tree);
		if (token->type != kNewLine) {
			NotifyAboutError(args, token, kFuncDeclNewLineExpectedError);
			TokenAdvOnNewLine(args->tree);
			RETURN 0;
		}

		RETURN 0;
	}

	int VarDeclaration(CreateNodeArgs* args, bool is_global_var) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		CATCH_ERROR(NewNode(&args->node));
		args->node->token = GetPreviousToken(args->tree);
		Token* token = GetToken(args->tree);

		if (token->type == kNewLine) {
			if (is_global_var) {
				Token* token = args->node->token;
				Var var = {token->val.name, (VarType)(token - 1)->val.id};
				CATCH_ERROR(VectorPushback(args->tree->global_vars, &var, sizeof(Var)));
			}
		} else if (token->type == kEqual) {
			CATCH_ERROR(VarDeclarationCaseEqual(args, is_global_var));
			RETURN 0;
		} else {
			NotifyAboutError(args, token, kVarDeclError);
			TokenAdvOnNewLine(args->tree);
		}

		RETURN 0;
	}

	int FuncConstructAddVar(CreateNodeArgs* args, Function* func) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		assert(func != NULL);

	  Token* token = GetToken(args->tree);
	  Var var = {};
	  if (token->type == kDecl) {
	  	var.type = (VarType)token->val.id;
	  } else {
	  	NotifyAboutError(args, token, kVarDeclExpectedError);
	  	RETURN 0;
	  }

	  token = GetToken(args->tree);
	  if (token->type == kVar) {
	  	var.name = token->val.name;
	  } else {
	  	NotifyAboutError(args, token, kVarNameExpectedError);
	  	RETURN 0;
	  }

	  CATCH_ERROR(VectorPushback(func->args, &var, sizeof(Var)));
	  ++func->args_num;

	  RETURN 0;
	}

	int FuncConstruct(CreateNodeArgs* args, Function* func) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		assert(func != NULL);

		CATCH_ERROR(NewVector(&func->args, sizeof(Var)));
		CATCH_ERROR(NewVector(&func->local_vars, sizeof(Var)));

		Token* token = GetTokenWithoutAdvancing(args->tree);
		if (token->type == kParenthC) {
			++args->tree->token_idx;
			RETURN 0;
		}

		bool is_end = false;
		while (!is_end) {
			CATCH_ERROR(FuncConstructAddVar(args, func));
			if (args->is_error_occured) {
				RETURN 0;
			}
			token = GetToken(args->tree);
			if (token->type == kParenthC) {
				is_end = true;
			} else if (token->type != kComma) {
				NotifyAboutError(args, token, kCommaParenthExpectedError);
				RETURN 0;
			} // else it is comma, then just continue
		}

		RETURN 0;
	}

	int FuncDeclaration(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		CATCH_ERROR(NewNode(&args->node));
		args->node->token = (Token*)VectorGet(args->tree->tokens, args->tree->token_idx - 1, sizeof(Token));
		Token* token = GetToken(args->tree);
		if (token->type != kParenthO) {
			RETURN kFatalError;
		}

		Function func = {};
		CATCH_ERROR(FuncConstruct(args, &func));
		CATCH_ERROR(VectorPushback(args->tree->functions, &func, sizeof(Function)));
		if (args->is_error_occured) {
			ProcessErrorSkippingStatementBodyIfAny(args->tree);
			RETURN 0;
		}

		CATCH_ERROR(CreateNode::StatementBody(args));
		RETURN 0;
	}

	int Declaration(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		Token* token = GetToken(args->tree);
		if (token->type == kVar) {
			CATCH_ERROR(CreateNode::VarDeclaration(args, true));
		} else if (token->type == kFunc) {
			CATCH_ERROR(CreateNode::FuncDeclaration(args));
		} else {
			NotifyAboutError(args, token, kFuncVarDeclErrorNameExpected);
			ProcessErrorSkippingStatementBodyIfAny(args->tree);
		}

		RETURN 0;
	}
}

int ProcessErrorSkippingStatementBodyIfAny(TreeInfo* tree) {
	$;
	assert(tree != NULL);

	bool is_open_brack_found_near = false;
	FindNearestOpenBracket(tree, &is_open_brack_found_near);
	if (is_open_brack_found_near) {
		bool is_error_occured = false;
		bool is_error_processed = false;
		CreateNodeArgs args = {tree, tree->root, false};
		CATCH_ERROR(CreateNode::StatementBody(&args));
	}

	RETURN 0;
}

void ProcessErrorSkippingToNewLineOrCloseBracket(TreeInfo* tree) {
	$;
	assert(tree != NULL);
	while (true) {
		assert(tree->token_idx < tree->tokens->sz && "This error is weird, probably file was damaged");
		Token* token = GetToken(tree);
		if (token->type == kBrackC) {
			GetTokenStepBack(tree);
			RETURN;
		} else if (token->type == kNewLine) {
			RETURN;
		}
	}
}

int RootConstructCaseDecl(TreeInfo* tree, bool* syntax_error_occured) {
	$;
	assert(tree != NULL);
	assert(syntax_error_occured != NULL);
	CreateNodeArgs args = {tree, NULL, false};
	CATCH_ERROR(CreateNode::Declaration(&args));

	if (!args.is_error_occured) {
		args.node->parent = tree->root;
		CATCH_ERROR(VectorPushback(tree->root->children, &args.node, sizeof(Node*)));
	} else {
		*syntax_error_occured = true;
	}

	RETURN 0;
}

int TreeConstruct(TreeInfo* tree) {
	$;
	assert(tree != NULL);

	CATCH_ERROR(NewNode(&tree->root));
	bool syntax_error_occured = false;

	while (tree->token_idx < tree->tokens->sz) {
		Token* token = GetToken(tree);
		switch (token->type) {
			case kDecl:
				CATCH_ERROR(RootConstructCaseDecl(tree, &syntax_error_occured));
				break;
			default:
				syntax_error_occured = true;
				PrintSyntaxError(tree->text, token, kFuncVarDeclExpectedError);
				ProcessErrorSkippingStatementBodyIfAny(tree);
				break;
		}
	}

	RETURN syntax_error_occured ? kSyntaxError : 0;
}

int TreeInfoConstructVector(Vector** vec_ptr,
Vector* not_freed_ptrs, size_t type_sz) {
	$;
	CATCH_ERROR(NewVector(vec_ptr, type_sz));
	CATCH_ERROR(VectorPushback(not_freed_ptrs, &((*vec_ptr)->arr), sizeof((*vec_ptr)->arr)));
	CATCH_ERROR(VectorPushback(not_freed_ptrs, vec_ptr, sizeof(*vec_ptr)));
	RETURN 0;
}

int TreeInfoConstruct(TreeInfo* tree_info, Code* code,
Vector* not_freed_ptrs) {
	$;
	assert(tree_info != NULL);
	assert(code != NULL);

	tree_info->text = code->text;
	tree_info->tokens = code->tokens;
	CATCH_ERROR(TreeInfoConstructVector(&tree_info->global_vars, not_freed_ptrs, sizeof(Var)));
	CATCH_ERROR(TreeInfoConstructVector(&tree_info->functions,   not_freed_ptrs, sizeof(Function)));

	if (GetTokenWithoutAdvancing(tree_info)->type == kNewLine) {
		++tree_info->token_idx;
	}

	RETURN 0;
}

void NodeDump(Node* node) {
	$;
	printf("NodeDump:\n");
	printf("node = %p\n", node);
	if (node == NULL) {
		RETURN;
	}
	printf("node parent = %p\n", node->parent);
	printf("token = %p\n", node->token);
	if (node->token == NULL) {
		RETURN;
	}
	printf("token type = %d\n", (int)node->token->type);
	printf("token val = %s\n", node->token->val.name);
	printf("token pos = %s\n%lu\n", node->token->pos.ofs, node->token->pos.line);
	$$;
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
			PrintErrorInfo((RuntimeError)err);\
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

	TreeInfo tree_info = {};
	CHECK_FOR_ERROR(TreeInfoConstruct(&tree_info, &code, &not_freed_ptrs));
	CHECK_FOR_ERROR(TreeConstruct(&tree_info));

	Dot::TreeDump(tree_info.root);

	printf("compilation finished successfully\n");
	RETURN 0;
}