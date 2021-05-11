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

void NodeDump(Node* node, const char* str) {
	$;
	assert(str != NULL);
	printf("%s:\n", str);
	printf("------------------------\n");
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
	switch (node->token->type) {
		case kVar:
		case kFunc:
			printf("name = %s\n", node->token->val.name);
			break;
		case kIntNum:
			printf("int_val = %d\n", node->token->val.int_val);
			break;
		case kFloatNum:
			printf("float_val = %f\n", node->token->val.float_val);
			break;
		default:
			printf("id = %d\n", node->token->val.id);
	}
	
	printf("token pos = %s\n%lu\n", node->token->pos.ofs, node->token->pos.line);
	printf("------------------------\n\n");
	$$;
}

void Swap(void* ptr0, void* ptr1, size_t sz) {
	$;
	char temp[sz];
	memcpy(temp, ptr0, sz);
	memcpy(ptr0, ptr1, sz);
	memcpy(ptr1, temp, sz);
	$$;
}

namespace dot {
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
					if ((node->token - 1)->type == kDecl) {
						sprintf(value, " | %s | %s", decl_types[(node->token - 1)->val.id], node->token->val.name);
					} else {
						sprintf(value, " | %s", node->token->val.name);
					}
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

	void NodeDump(Node* node, FILE* file, Node* parent) {
		$;
		assert(file != NULL);
	  fprintf(file, "edge [color=black];\n");
	  fprintf(file, "\"%p\"->\"%p\"\n", node, node->parent);
	  if (parent != NULL) {
	    fprintf(file, "edge [color=red];\n");
		  fprintf(file, "\"%p\"->\"%p\"\n", parent, node);
		}
		$$;
	}

	void SubtreeDump(Node* node, FILE* file, Node* parent) {
		$;
		assert(file != NULL);

		NodeDeclare(node, file);
		if (node == NULL) {
			RETURN;
		}

		for (size_t i = 0; i < node->children->sz; ++i) {
			Node* child = *(Node**)VectorGet(node->children, i, sizeof(Node*));
			SubtreeDump(child, file, node);
		}
		NodeDump(node, file, parent);
		$$;
	}

	void TreeDump(Node* root) {
		$;
		assert(root != NULL);
		static int cnt = -1;
		++cnt;

		char file_name[10] = {};
		sprintf(file_name, "d%d", cnt);
		FILE* file = fopen(file_name, "w");
		fprintf(file, "digraph G {\n");
		fprintf(file, "node [shape = \"record\","
			            " style = \"filled\", fillcolor = "
			            "\"%s\"]\n", kRGBColorBlue);
		SubtreeDump(root, file, NULL);
		fprintf(file, "}\n");

		fclose(file);

		char cmd[50] = {};
		sprintf(cmd, "dot -Tpng -O %s", file_name);
		system(cmd);
		sprintf(cmd, "xdg-open %s.png", file_name);
	  system(cmd);
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

int NewNode(Node** node, Token* token) {
	$;
	assert(node != NULL);
	assert(*node == NULL);
	*node = (Node*)calloc(1, sizeof(Node));
	if (*node == NULL) {
		RETURN kNotEnoughMemory;
	}
	CATCH_ERROR( NewVector(&((*node)->children), sizeof(Node*)) );
	if (token != NULL) {
		(*node)->token = token;
	}
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

int TokenAdvOnNewLine(TreeInfo* tree) {
	$;
	assert(tree != NULL);
	Token* token = GetToken(tree);
	while (token->type != kNewLine && tree->token_idx < tree->tokens->sz) {
		token = GetToken(tree);
	}
	RETURN 0;
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
			token = GetTokenWithoutAdvancing(tree);
			if (token->type == kBrackO) {
				*is_open_brack_found_near = true;
				RETURN;
			} else {
				RETURN;
			}
		}
	}
}

int ProcessErrorSkippingStatementBodyIfAny(TreeInfo* tree);
// skips to new line or end of statement body whichether comes first
int SkipToNewLineOrEndOfStatementBody(TreeInfo* tree);

struct CreateNodeArgs {
	TreeInfo* tree = NULL;
	Node* node = NULL;
	bool is_error_occured = false;
};

CreateNodeArgs MakeNewArgs(CreateNodeArgs* old_args) {
	$;
	assert(old_args != NULL);
	CreateNodeArgs args = {};
	args.tree = old_args->tree;
	RETURN args;
}

int ProcessCreatedNode(CreateNodeArgs* new_args, CreateNodeArgs* old_args) {
	$;
	if (new_args->node != NULL) {
		CATCH_ERROR(VectorPushback(old_args->node->children, &new_args->node, sizeof(Node*)));
		new_args->node->parent = old_args->node;
	}
	RETURN 0;
}

struct ProcessErrorArgs {
	CreateNodeArgs* create_node_args = NULL;
	Token* token = NULL;
	SyntaxError err = kUnknownSyntaxError;
	int (*func)(TreeInfo*) = NULL;
};
int ProcessError(ProcessErrorArgs args) {
	$;
	assert(args.create_node_args != NULL);
	TreeInfo* tree = args.create_node_args->tree;
	assert(tree != NULL);
	assert(args.token != NULL);

	GetTokenStepBack(tree);
  args.create_node_args->is_error_occured = true;
	PrintSyntaxError(tree->text, args.token, args.err);
	if (args.func != NULL) {
		CATCH_ERROR(args.func(tree));
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
		ProcessError({args, token, kVarDeclExpectedError, NULL});
  	RETURN 0;
  }

  token = GetToken(args->tree);
  if (token->type == kVar) {
  	var.name = token->val.name;
  } else {
		ProcessError({args, token, kVarNameExpectedError, NULL});
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
			ProcessError({args, token, kCommaParenthExpectedError, NULL});
			is_end = true;
		} // else it is comma, then just continue
	}

	RETURN 0;
}

bool IsPlusOrMinus(Token* token) {
	$;
	assert(token != NULL);

	if (token->type != kArithOperator) {
		RETURN false;
	}

	int id = token->val.id;
	RETURN id == kPlus || id == kMinus;
}

int BranchNewNodeFromArgs(CreateNodeArgs* args, int (*func)(CreateNodeArgs*));

namespace create_node {
	int VarDeclaration(CreateNodeArgs* args, bool is_global_var);
	int Statement(CreateNodeArgs* args);
	int FuncCall(CreateNodeArgs* args);
	int Expr(CreateNodeArgs* args);

	// creates a new node
	int Primary(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);

		Token* token = GetToken(args->tree);
		switch (token->type) {
			case kIntNum:
			case kFloatNum:
			case kVar:
				CATCH_ERROR(NewNode(&args->node, token));
				break;

			case kFunc:
				CATCH_ERROR(NewNode(&args->node, token));
				CATCH_ERROR(create_node::FuncCall(args));
				break;

			case kParenthO:
				CATCH_ERROR(create_node::Expr(args));
				// CATCH_ERROR(NewNode(&args->node, token));
				// CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Expr));
				if (GetToken(args->tree)->type != kParenthC) {
					ProcessError({args, token, kParenthCloseExpectedError, NULL});
				}
				break;

			default:
				ProcessError({args, token, kExpressionExpectedError, NULL});
		}

		RETURN 0;
	}

	// creates a new node
	int Expr(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		CATCH_ERROR(create_node::Primary(args));
		if (args->is_error_occured) { RETURN 0; }

		Token* token = GetToken(args->tree);
		if (!IsPlusOrMinus(token)) {
			GetTokenStepBack(args->tree);
			RETURN 0;
		}

		Node* prim_node = NULL;
		CATCH_ERROR(NewNode(&prim_node, token));
		Swap(prim_node, args->node, sizeof(Node));
		CATCH_ERROR(VectorPushback(args->node->children, &prim_node, sizeof(Node*)));
		CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Expr));
		RETURN 0;
	}

	int FuncCall(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);

		if (GetToken(args->tree)->type != kParenthO) {
			RETURN kFatalError;
		}

		Token* token = GetTokenWithoutAdvancing(args->tree);
		if (token->type == kParenthC) {
			++args->tree->token_idx;
			RETURN 0;
		}

		CreateNodeArgs new_args = {};
		bool is_end = false;
		while (!is_end) {
			CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Expr));
			if (args->is_error_occured) { RETURN 0; }

			token = GetToken(args->tree);
			if (token->type == kParenthC) {
				is_end = true;
			} else if (token->type != kComma) {
				ProcessError({args, token, kCommaParenthExpectedError, NULL});
				is_end = true;
			}
		} // else it is comma, then just continue

		RETURN 0;
	}

  // creates a node with binary operation token
	int Inequality(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		CATCH_ERROR(NewNode(&args->node, NULL));
		CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Expr));
		if (args->is_error_occured) { RETURN 0; }

		Token* token = GetToken(args->tree);
		args->node->token = token;
		if (token->type != kComp) {
			ProcessError({args, token, kCompSignExpectedError, NULL});
			RETURN 0;
		}

		CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Expr));
		RETURN 0;
	}

	// int LogicExpr(CreateNodeArgs* args) {
	// 	$;
	// 	assert(args != NULL);
	// 	assert(args->tree != NULL);
	// 	CATCH_ERROR(NewNode(&args->node, NULL));
	// 	CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Expr));
	// 	if (args->is_error_occured) { RETURN 0; }

	// 	Token* token = GetToken(args->tree);
	// 	args->node->token = token;
	// 	if (token->type != kAnd) {
	// 		RETURN 0;
	// 	}

	// 	CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Expr));
	// 	RETURN 0;
	// }

	int Condition(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		Token* token = GetToken(args->tree);
		if (token->type != kParenthO) {
			ProcessError({args, token, kParenthOpenExpectedError, NULL});
			RETURN 0;
		}

		CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Inequality));
		if (!args->is_error_occured && GetToken(args->tree)->type != kParenthC) {
			ProcessError({args, token, kParenthCloseExpectedError, NULL});
		}
		RETURN 0;
	}

	int StatementBody(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		Token* token = GetToken(args->tree);
		if (token->type != kBrackO) {
			ProcessError({args, token, kBracketOpenExpectedError, NULL});
			RETURN 0;
		}

		SkipNewLineIfAny(args->tree);
		bool is_end = false;
		while (!is_end) {
			token = GetTokenWithoutAdvancing(args->tree);
			if (token->type == kBrackC) {
				++args->tree->token_idx;
				is_end = true;
			} else {
				CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Statement));
			}
			SkipNewLineIfAny(args->tree);
		}

		RETURN 0;
	}

	namespace statement {
		int Conditional(CreateNodeArgs* args) {
			$;
			assert(args != NULL);
			assert(args->tree != NULL);
			CATCH_ERROR(create_node::Condition(args));
			if (args->is_error_occured) {
				ProcessErrorSkippingStatementBodyIfAny(args->tree);
				RETURN 0;
			}
			CATCH_ERROR(create_node::StatementBody(args));
			RETURN 0;
		}

		// int Else(CreateNodeArgs* args) {
		// 	$;
		// 	assert(args != NULL);
		// 	assert(args->tree != NULL);
		// 	CATCH_ERROR(create_node::Condition(args));
		// 	if (args->is_error_occured) {
		// 		ProcessErrorSkippingStatementBodyIfAny(args->tree);
		// 		RETURN 0;
		// 	}
		// 	CATCH_ERROR(create_node::StatementBody(args));
		// 	RETURN 0;
		// }

		int Return(CreateNodeArgs* args) {
			$;
			assert(args != NULL);
			assert(args->tree != NULL);
			Token* token = GetTokenWithoutAdvancing(args->tree);
			if (token->type != kNewLine && token->type != kBrackC) {
				CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Expr));
				if (args->is_error_occured) { SkipToNewLineOrEndOfStatementBody(args->tree); RETURN 0; }

				token = GetToken(args->tree);
				if (token->type != kNewLine && token->type != kBrackC) {
					ProcessError({args, token, kStatementNewLineExpectedError, SkipToNewLineOrEndOfStatementBody});
				} else {
					GetTokenStepBack(args->tree);
				}
				RETURN 0;
			} 

			if (token->type == kNewLine) {
				++args->tree->token_idx;
			}
			RETURN 0;
		}

		int Var(CreateNodeArgs* args) {
			$;
			assert(args != NULL);
			assert(args->tree != NULL);

			Token* token = GetToken(args->tree);
			if (token->type != kEqual) {
				ProcessError({args, token, kEqualAfterVarExpectedError, SkipToNewLineOrEndOfStatementBody});
				RETURN 0;
			}
			CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Expr));
			if (args->is_error_occured) { SkipToNewLineOrEndOfStatementBody(args->tree); }

			RETURN 0;
		}

		int VarDeclaration(CreateNodeArgs* args) {
			$;
			assert(args != NULL);
			assert(args->tree != NULL);

			Token* token = GetToken(args->tree);
			args->node->token = token;
			if (token->type != kVar) {
				ProcessError({args, token, kVarNameExpectedError, SkipToNewLineOrEndOfStatementBody});
				RETURN 0;
			}

			CATCH_ERROR(create_node::VarDeclaration(args, false));
			if (args->is_error_occured) { SkipToNewLineOrEndOfStatementBody(args->tree); RETURN 0; }
			
			token = GetToken(args->tree);
			if (token->type != kNewLine && token->type != kBrackC) {
				ProcessError({args, token, kStatementNewLineExpectedError, SkipToNewLineOrEndOfStatementBody});
			} else {
				GetTokenStepBack(args->tree);
			}
			RETURN 0;
		}

		int Func(CreateNodeArgs* args) {
			$;
			assert(args != NULL);
			assert(args->tree != NULL);

			CATCH_ERROR(create_node::FuncCall(args));
			if (args->is_error_occured) { SkipToNewLineOrEndOfStatementBody(args->tree); }

			RETURN 0;
		}

		int LoopJump(CreateNodeArgs* args) {
			$;
			assert(args != NULL);
			assert(args->tree != NULL);
			Token* token = GetToken(args->tree);
			if (token->type != kNewLine && token->type != kBrackC) {
				ProcessError({args, token, kStatementNewLineExpectedError, SkipToNewLineOrEndOfStatementBody});
			} else {
				GetTokenStepBack(args->tree);
			}
			RETURN 0;
		}
	}

  // creates a node with statement token
	int Statement(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
	  Token* token = GetToken(args->tree);
	  CATCH_ERROR(NewNode(&args->node, token));
		switch (token->type) {
			case kElseIf:
			case kIf:
			case kWhile:
				CATCH_ERROR(create_node::statement::Conditional(args));
				break;

			case kElse:
				CATCH_ERROR(create_node::StatementBody(args));
				break;

			case kReturn:
				CATCH_ERROR(create_node::statement::Return(args));
				break;

			case kVar:
				CATCH_ERROR(create_node::statement::Var(args));
				break;

			case kDecl:
				CATCH_ERROR(create_node::statement::VarDeclaration(args));
				break;

			case kFunc:
				CATCH_ERROR(create_node::statement::Func(args));
				break;

			case kBreak:
			case kContinue:
				CATCH_ERROR(create_node::statement::LoopJump(args));
				break;

			default:
				ProcessError({args, token, kStatementExpectedError, SkipToNewLineOrEndOfStatementBody});
		}
		RETURN 0;
	}

	int VarDeclaration(CreateNodeArgs* args, bool is_global_var) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);

		Token* token = GetToken(args->tree);
		if (is_global_var) {
		  Token* node_token = args->node->token;
		  Var var = {node_token->val.name, (VarType)(node_token - 1)->val.id};
		  CATCH_ERROR(VectorPushback(args->tree->global_vars, &var, sizeof(Var)));
		}

		if (token->type == kNewLine) {
			GetTokenStepBack(args->tree);
		} else if (token->type == kEqual) {
			CATCH_ERROR(BranchNewNodeFromArgs(args, create_node::Expr));
		} else {
			ProcessError({args, token, kVarDeclError, NULL});
		}

		RETURN 0;
	}

	int FuncDeclaration(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);

		Token* token = GetToken(args->tree);
		if (token->type != kParenthO) {
			RETURN kFatalError;
		}

		Function func = {};
		func.name = ((Token*)VectorGet(args->tree->tokens, args->tree->token_idx - 2, sizeof(Token)))->val.name;
		CATCH_ERROR(FuncConstruct(args, &func));
		CATCH_ERROR(VectorPushback(args->tree->functions, &func, sizeof(Function)));
		if (args->is_error_occured) {
			ProcessErrorSkippingStatementBodyIfAny(args->tree);
			RETURN 0;
		}

		CATCH_ERROR(create_node::StatementBody(args));
		RETURN 0;
	}

	// creates a new node with token var or func
	int Declaration(CreateNodeArgs* args) {
		$;
		assert(args != NULL);
		assert(args->tree != NULL);
		Token* token = GetToken(args->tree);
		CATCH_ERROR(NewNode(&args->node, token));
		if (token->type == kVar) {
			CATCH_ERROR(create_node::VarDeclaration(args, true));
			if (args->is_error_occured) {
				TokenAdvOnNewLine(args->tree);
				RETURN 0;
			}
		  token = GetToken(args->tree);
		  if (token->type != kNewLine) {
		  	ProcessError({args, token, kVarDeclNewLineExpectedError, TokenAdvOnNewLine});
		  }
		} else if (token->type == kFunc) {
			CATCH_ERROR(create_node::FuncDeclaration(args));
		} else {
			ProcessError({args, token, kFuncVarDeclErrorNameExpected, ProcessErrorSkippingStatementBodyIfAny});
		}

		RETURN 0;
	}
}

int BranchNewNodeFromArgs(CreateNodeArgs* args, int (*func)(CreateNodeArgs*)) {
	$;
	assert(args != NULL);
	assert(args->node != NULL);
	assert(func != NULL);

	CreateNodeArgs new_args = {};
	new_args.tree = args->tree;
	CATCH_ERROR(func(&new_args));
	if (new_args.node != NULL) {
		CATCH_ERROR(VectorPushback(args->node->children, &new_args.node, sizeof(Node*)));
	}
	if (new_args.is_error_occured) {
		args->is_error_occured = true;
	}

	RETURN 0;
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
		CATCH_ERROR(create_node::StatementBody(&args));
	}

	RETURN 0;
}

// skips to new line or end of statement body whichether comes first
int SkipToNewLineOrEndOfStatementBody(TreeInfo* tree) {
	$;
	assert(tree != NULL);
	int bracket_count = 0;
	while (true) {
		assert(tree->token_idx < tree->tokens->sz && "This error is weird, probably file was damaged");
		Token* token = GetToken(tree);
		if (token->type == kBrackO) {
			++bracket_count;
		} else if (token->type == kBrackC) {
			--bracket_count;
			if (bracket_count < 0) {
			  GetTokenStepBack(tree);
			  RETURN 0;
			}
		} else if (token->type == kNewLine) {
			RETURN 0;
		}
	}
	RETURN 0;
}

int RootConstructCaseDecl(TreeInfo* tree, bool* syntax_error_occured) {
	$;
	assert(tree != NULL);
	assert(syntax_error_occured != NULL);
	CreateNodeArgs args = {tree, NULL, false};
	CATCH_ERROR(create_node::Declaration(&args));
	if (args.node != NULL) {
		CATCH_ERROR(VectorPushback(tree->root->children, &args.node, sizeof(Node*)));
	}
	if (args.is_error_occured) {
		*syntax_error_occured = true;
	}
	RETURN 0;
}

int TreeConstruct(TreeInfo* tree) {
	$;
	assert(tree != NULL);

	CATCH_ERROR(NewNode(&tree->root, NULL));
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

void TieParents(Node* subroot, Node* parent) {
	$;
	assert(subroot != NULL);
	subroot->parent = parent;
	for (size_t i = 0; i < subroot->children->sz; ++i) {
		TieParents(*(Node**)VectorGet(subroot->children, i, sizeof(Node*)), subroot);
	}
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
	
	// TokenDump(code.tokens, code.text);
	// FreeAllMemory(&not_freed_ptrs);

	TreeInfo tree_info = {};
	CHECK_FOR_ERROR(TreeInfoConstruct(&tree_info, &code, &not_freed_ptrs));
	CHECK_FOR_ERROR(TreeConstruct(&tree_info));
	TieParents(tree_info.root, NULL);
	dot::TreeDump(tree_info.root);
	// FunctionsAndGlobalVarsDump(tree_info.global_vars, tree_info.functions);

	printf("compilation finished successfully\n");
	RETURN 0;
}