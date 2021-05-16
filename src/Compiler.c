#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "../include/OutputAndError.h"
#include "../include/StackTrace.h"
#include "../include/Vector.h"
#include "../include/Tokenize.h"
#include "../include/Tree.h"
#include "../include/Compiler.h"
#include "../include/main.h"

// data->is_error_occured = true;
// PrintCompilationError(data->tree->text, node->token, kVarInitInvalidTypeError);

static int ER_ = 0;  // this variable is auxiliary and used only in macro
#define CATCH_ERROR(func)   \
  if ((ER_ = func) != 0) {  \
    RETURN ER_;             \
  }

namespace print {
	void Tabs(FILE* file, size_t tabs_num) {
		$;
		for (size_t i = 0; i < tabs_num; ++i) {
			fprintf(file, "\t");
		}
		$$;
	}

	// void AddFirstRegVar(CompilerData* data, CompilerArgs* args,
	// Var* var, bool is_global) {
	// 	assert(var->type == kVarInt || var->type == kVarFloat);
	// 	print::Tabs(data->file, args->tabs_num);
	// 	if (type == kIntNodeType) {
	// 		if (is_global) {
	// 			fprintf(data->file, "add eax, [%s]\n", var->name);
	// 		} else {
	// 			fprintf(data->file, "add eax, [rbp + %d]\n", var->stack_ofs);
	// 		}
	// 	} else {
	// 		if (is_global) {
	// 			fprintf(data->file, "addss xmm0, [%s]\n", var->name);
	// 		} else {
	// 			fprintf(data->file, "addss xmm0, [rbp + %d]\n", var->stack_ofs);
	// 		}
	// 	}
	// }

	void PopToFirstReg(CompilerData* data, CompilerArgs* args,
	NodeType type) {
		assert(type == kIntNodeType || type == kFloatNodeType);
		print::Tabs(data->file, args->tabs_num);
		if (type == kIntNodeType) {
			fprintf(data->file, "mov eax, [rsp]\n");
		} else {
			fprintf(data->file, "movss xmm0, [rsp]\n");
		}
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "add rsp, 4\n");
	}

	// do just pop reg here

	// void PopToFloatReg(CompilerData* data, CompilerArgs* args,
	// Register reg) {
	// 	assert(reg == kXmm0 || reg == kXmm1);
	// 	const char* arr[] = {"xmm0", "xmm1"};
	// 	print::Tabs(data->file, args->tabs_num);
	// 	fprintf(data->file, "movss %s, [rsp]\n", arr[reg - kXmm0]);
	// 	print::Tabs(data->file, args->tabs_num);
	// 	fprintf(data->file, "add rsp, 4\n");
	// }

	// void PopToIntReg(CompilerData* data, CompilerArgs* args,
	// Register reg) {
	// 	assert(reg == kEax || reg == kEbx);
	// 	const char* arr[] = {"eax", "ebx"};
	// 	print::Tabs(data->file, args->tabs_num);
	// 	fprintf(data->file, "mov %s, [rsp]\n", arr[reg - kEax]);
	// 	print::Tabs(data->file, args->tabs_num);
	// 	fprintf(data->file, "add rsp, 4\n");
	// }

	void PopReg(CompilerData* data, CompilerArgs* args,
	Register reg) {
		assert(reg == kEax || reg == kEbx || reg == kXmm0 || reg == kXmm1);
		const char* arr[] = {"eax", "ebx", "xmm0", "xmm1"};
		print::Tabs(data->file, args->tabs_num);
		if (reg <= kEbx) {
			fprintf(data->file, "mov %s, [rsp]\n", arr[(int)reg]);
		} else {
			fprintf(data->file, "movss %s, [rsp]\n", arr[(int)reg]);
		}
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "add rsp, 4\n");
	}

	void PushReg(CompilerData* data, CompilerArgs* args,
	Register reg) {
		assert(reg == kEax || reg == kEbx || reg == kXmm0 || reg == kXmm1);
		const char* arr[] = {"eax", "ebx", "xmm0", "xmm1"};
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "sub rsp, 4\n");
		print::Tabs(data->file, args->tabs_num);
		if (reg <= kEbx) {
			fprintf(data->file, "mov [rsp], %s\n", arr[(int)reg]);
		} else {
			fprintf(data->file, "movss [rsp], %s\n", arr[(int)reg]);
		}
	}

	void AddFirstRegStackTopAndPop(CompilerData* data, CompilerArgs* args,
	NodeType node_type) {
		assert(node_type == kIntNodeType || node_type == kFloatNodeType);
		print::Tabs(data->file, args->tabs_num);
		if (node_type == kIntNodeType) {
			fprintf(data->file, "add eax, [rsp]\n");
		} else {
			fprintf(data->file, "addss xmm0, [rsp]\n");
		}
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "add rsp, 4\n");
	}

	void PushVar(CompilerData* data, CompilerArgs* args,
	Var* var, bool is_global) {
		assert(var->type == kVarInt || var->type == kVarFloat);
		print::Tabs(data->file, args->tabs_num);
		if (var->type == kVarInt) {
			if (is_global) {
				fprintf(data->file, "mov eax, [%s]\n", var->name);	
			} else {
				fprintf(data->file, "mov eax, [rbp + %d]\n", var->stack_ofs);	
			}
			print::PushReg(data, args, kEax);
		} else {			
			if (is_global) {
				fprintf(data->file, "movss xmm0, [%s]\n", var->name);	
			} else {
				fprintf(data->file, "movss xmm0, [rbp + %d]\n", var->stack_ofs);	
			}
			print::PushReg(data, args, kXmm0);
		}
	}

	void PopToVar(CompilerData* data, CompilerArgs* args,
	Var* var, bool is_global) {
		assert(var->type == kVarInt || var->type == kVarFloat);
		if (var->type == kVarInt) {
			print::Tabs(data->file, args->tabs_num);
			fprintf(data->file, "mov eax, [rsp]\n");
			if (is_global) {
				print::Tabs(data->file, args->tabs_num);
				fprintf(data->file, "mov [%s], eax\n", var->name);	
			} else {
				print::Tabs(data->file, args->tabs_num);
				fprintf(data->file, "mov [rbp + %d], eax\n", var->stack_ofs);	
			}
		} else {			
			print::Tabs(data->file, args->tabs_num);
			fprintf(data->file, "movss xmm0, [rsp]\n");
			if (is_global) {
				print::Tabs(data->file, args->tabs_num);
				fprintf(data->file, "movss [%s], xmm0\n", var->name);	
			} else {
				print::Tabs(data->file, args->tabs_num);
				fprintf(data->file, "movss [rbp + %d], xmm0\n", var->stack_ofs);	
			}
		}
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "add rsp, 4\n");
	}

	void PushConst(CompilerData* data, CompilerArgs* args, int val) {
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "mov eax, %d\n", val);
		print::PushReg(data, args, kEax);
	}

	void PushConst(CompilerData* data, CompilerArgs* args, float val) {
		VectorPushback(data->consts, &val, sizeof(float));
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "movss xmm0, [const%lu]\n", data->consts->sz);
		print::PushReg(data, args, kXmm0);
	}

	void AddEaxEbx(CompilerData* data, CompilerArgs* args) {
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "add eax, ebx\n");
	}

	void XorFirstReg(CompilerData* data, CompilerArgs* args,
	NodeType node_type) {
		assert(node_type == kIntNodeType || node_type == kFloatNodeType);
		print::Tabs(data->file, args->tabs_num);
		if (node_type == kIntNodeType) {
			fprintf(data->file, "xor eax, eax\n");
		} else {
			fprintf(data->file, "pxor xmm0, xmm0\n");
		}
	}

	void XorEaxEax(CompilerData* data, CompilerArgs* args) {
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "xor eax, eax\n");
	}

	void Leave(CompilerData* data, CompilerArgs* args) {
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "leave\n");
	}

	void Ret(CompilerData* data, CompilerArgs* args) {
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "ret\n");
	}

	void CallFunc(CompilerData* data, CompilerArgs* args,
	const char* name) {
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "call %s\n", name);
	}

	void MoveVarToSecondReg(CompilerData* data, CompilerArgs* args,
	Var* var, bool is_global) {
		assert(var->type == kVarInt || var->type == kVarFloat);
		print::Tabs(data->file, args->tabs_num);
		if (is_global) {
			if (var->type == kVarInt) {
				fprintf(data->file, "mov ebx, [%s]\n", var->name);
			} else {
				fprintf(data->file, "movss xmm1, [%s]\n", var->name);
			}
		} else {
			if (var->type == kVarInt) {
				fprintf(data->file, "mov ebx, [rbp + %d]\n", var->stack_ofs);
			} else {
				fprintf(data->file, "movss xmm1, [rbp + %d]\n", var->stack_ofs);
			}
		}
	}

	void MoveIntToSecondReg(CompilerData* data, CompilerArgs* args,
	int val) {
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "mov ebx, %d\n", val);
	}

	void MoveFloatToSecondReg(CompilerData* data, CompilerArgs* args,
	float val) {
		VectorPushback(data->consts, &val, sizeof(float));
		print::Tabs(data->file, args->tabs_num);
		fprintf(data->file, "movss xmm1, [const%lu]\n", data->consts->sz);
	}

	void PushFuncRetVal(CompilerData* data, CompilerArgs* args, Function* func) {
		assert(func->type == kFuncInt || func->type == kFuncFloat);
		if (func->type == kFuncInt) {
			print::PushReg(data, args, kEax);
		} else {
			print::PushReg(data, args, kXmm0);
		}
	}

	void CleanFuncArgumentsFromStack(CompilerData* data, CompilerArgs* args,
	Function* func) {
		int adder = 0;
		if (func->args->sz > 0) {
			Var* var = (Var*)func->args->arr;
			adder = var->stack_ofs - 12;
		}
		if (adder != 0) {
			print::Tabs(data->file, args->tabs_num);
			fprintf(data->file, "add rsp, %d\n", adder);
		}
	}
}

Node* GetChild(Node* node, size_t idx) {
	$;
	assert(node != NULL);
	RETURN *(Node**)VectorGet(node->children, idx, sizeof(Node*));
}

CompilerArgs MakeNewArgs(CompilerArgs* old_args, Node* node) {
	$;
	CompilerArgs args = {};
	memcpy(&args, old_args, sizeof(CompilerArgs));
	args.cur_node = node;
	RETURN args;
}

Node* GetLeftmostNode(Node* node) {
	$;
	assert(node != NULL);
	while (true) {
		if (node->children->sz == 0) {
			RETURN node;
		}
		node = GetChild(node, 0);
	}
	RETURN NULL;
}

Function* FindFunction(Vector* functions, const char* name) {
	$;
	size_t sz = functions->sz;
	for (size_t i = 0; i < sz; ++i) {
		Function* func = (Function*)VectorGet(functions, i, sizeof(Function));
		if (strcmp(func->name, name) == 0) {
			RETURN func;
		}
	}
	RETURN NULL;
}

Var* FindVar(Vector* var_vector, const char* name) {
	$;
	size_t sz = var_vector->sz;
	for (size_t i = 0; i < sz; ++i) {
		Var* var = (Var*)VectorGet(var_vector, i, sizeof(Var));
		if (strcmp(var->name, name) == 0) {
			RETURN var;
		}
	}
	RETURN NULL;
}

void ProcessCompilationError(CompilerData* data, Token* token, CompilationError err) {
	$;
	data->is_error_occured = true;
	PrintCompilationError(data->tree->text, token, err);
	$$;
}

Var* FindVarGlobalOrLocalOrArgument(CompilerData* data, CompilerArgs* args,
Token* token, bool* is_global) {
	$;
	Var* var = FindVar(data->tree->global_vars, token->val.name);
	if (var != NULL) {
		*is_global = true;
		RETURN var;
	}
	var = FindVar(args->cur_func->local_vars, token->val.name);
	if (var != NULL) {
		RETURN var;
	}
	RETURN FindVar(args->cur_func->args, token->val.name);
}

bool DoTypesMatch(NodeType node_type, Var* var) {
	$;
	assert(var->type == kVarInt || var->type == kVarFloat);
	assert(node_type == kIntNodeType || node_type == kFloatNodeType);
	if (var->type == kVarInt) {
		RETURN node_type == kIntNodeType;
	}
	RETURN node_type == kFloatNodeType;
}

// bool DoTypesMatch(Token* token, Var* var) {
// 	$;
// 	assert(var->type == kVarInt || var->type == kVarFloat);
// 	assert(token->val.id == (VarType)kVarInt || token->val.id == (VarType)kVarFloat);
// 	RETURN var->type == (VarType)token->val.id;
// }

bool DoTypesMatch(NodeType node_type0, NodeType node_type1) {
	$;
	assert(node_type0 == kIntNodeType || node_type0 == kFloatNodeType);
	assert(node_type1 == kIntNodeType || node_type1 == kFloatNodeType);
	RETURN node_type0 == node_type1;
}

bool DoTypesMatch(FuncType func_type, VarType var_type) {
	$;
	switch (func_type) {
		case kFuncInt:
			RETURN var_type == kVarInt;
		case kFuncFloat:
			RETURN var_type == kVarFloat;
		default:
			assert(0);
	}
  RETURN false;
}

bool DoTypesMatch(Function* func, NodeType node_type) {
	$;
	assert(func->type == kFuncInt || func->type == kFuncFloat);
	if (func->type == kFuncInt) {
		RETURN node_type == kIntNodeType;
	}
  RETURN node_type == kFloatNodeType;
}

bool IsNumOfArgumentsCorrect(Function* func, size_t num) {
	return func->args_num == num;
}

bool CheckReturnCorrectness(CompilerData* data,
CompilerArgs* args) {
	$;
	Node* node = args->cur_node;
	assert(node != NULL);
	switch (node->children->sz) {
		case 0:
			if (args->cur_func->type != kFuncVoid) {
			  data->is_error_occured = true;
			  PrintCompilationError(data->tree->text, node->token, kVoidReturnError);
				RETURN false;
			}
			break;
		case 1:
			if (args->cur_func->type == kFuncVoid) {
				data->is_error_occured = true;
				PrintCompilationError(data->tree->text, node->token, kNonVoidReturnError);
				RETURN false;
			}
			break;
		default:
			assert(0);
	}
	RETURN true;
}

namespace compile {
	NodeType Expr(CompilerData* data, CompilerArgs* args);

	void ExprCasePlusProcessError(CompilerData* data, CompilerArgs* args, Node* child) {
		$;
    Node* node = GetLeftmostNode(child);
    Token* token = node->token - 1;
    for (; token->type == kParenthO; --token);
    data->is_error_occured = true;
    PrintCompilationError(data->tree->text, token,
    	                    kDifferentTypeOperandsError);
  	$$;
	}

	NodeType ExprCasePlus(CompilerData* data, CompilerArgs* args) {
		$;
		Node* node = args->cur_node;
		assert(node->token->val.id == (int)kPlus);

		size_t sz = node->children->sz;
		assert(sz >= 2);
		Node* child = GetChild(node, 0);
  	CompilerArgs new_args = MakeNewArgs(args, child);
  	NodeType node_type0 = compile::Expr(data, &new_args);
  	if (node_type0 == kUnknownNodeType) { RETURN kUnknownNodeType; }

		for (size_t i = 1; i < sz; ++i) {
			Node* child = GetChild(node, i);
	  	CompilerArgs new_args = MakeNewArgs(args, child);
	  	NodeType node_type = compile::Expr(data, &new_args);
	  	if (node_type == kUnknownNodeType) { RETURN kUnknownNodeType; }
	  	if (node_type != node_type0) {
	  		ExprCasePlusProcessError(data, args, child);
	  		RETURN kUnknownNodeType;
	  	}
		}

		print::XorFirstReg(data, args, node_type0);
		for (size_t i = 0; i < sz; ++i) {
			print::AddFirstRegStackTopAndPop(data, args, node_type0);
		}
		if (node_type0 == kIntNodeType) {
			print::PushReg(data, args, kEax);
		} else {
			print::PushReg(data, args, kXmm0);
		}
		RETURN node_type0;
	}

	bool FuncCallProcessChild(CompilerData* data, CompilerArgs* args,
	Node* child, Var* arg) {
		$;
  	CompilerArgs new_args = MakeNewArgs(args, child);
  	NodeType node_type = compile::Expr(data, &new_args);
  	if (node_type == kUnknownNodeType) { RETURN true; }
		if (!DoTypesMatch(node_type, arg)) {
			ProcessCompilationError(data, child->token, kIncorrecArgumentType);
			RETURN true;
		}
		RETURN false;
	}

	NodeType FuncCall(CompilerData* data, CompilerArgs* args,
	bool is_part_of_expr) {
		$;
		Node* node = args->cur_node;
		assert(node != NULL);

		Function* func = FindFunction(data->tree->functions, node->token->val.name);
		if (func == NULL) {
	    ProcessCompilationError(data, node->token, kNoSuchFuncError);
	    RETURN kUnknownNodeType;
		} 

		if (is_part_of_expr && func->type == kFuncVoid) {
	    ProcessCompilationError(data, node->token, kVoidFuncInExpr);
	    RETURN kUnknownNodeType;
		}

		size_t sz = node->children->sz;
		if (!IsNumOfArgumentsCorrect(func, sz)) {
	    ProcessCompilationError(data, node->token, kIncorrectArgumentsNumToFunc);
	    RETURN kUnknownNodeType;
		}

		for (size_t i = 0; i < sz; ++i) {
			Node* child = GetChild(node, i);
			Var* arg = (Var*)VectorGet(func->args, i, sizeof(Var));
			bool is_error_occured = FuncCallProcessChild(data, args, child, arg);
			if (is_error_occured) {
				RETURN kUnknownNodeType;
			}
		}
		print::CallFunc(data, args, func->name);
		print::CleanFuncArgumentsFromStack(data, args, func);
		if (is_part_of_expr) {
			print::PushFuncRetVal(data, args, func);
		}
		
		RETURN func->type == kFuncInt ? kIntNodeType : kFloatNodeType;
	}

	NodeType ExprCaseVar(CompilerData* data, CompilerArgs* args) {
		$;
		Node* node = args->cur_node;
		assert(node->token->type == kVar);
		bool is_global = false;
		Var* var = FindVarGlobalOrLocalOrArgument(data, args, node->token, &is_global);
		if (var == NULL) {
	    ProcessCompilationError(data, node->token, kNoSuchVarError);
	    RETURN kUnknownNodeType;
		}
		print::PushVar(data, args, var, is_global);
		RETURN var->type == kVarInt ? kIntNodeType : kFloatNodeType;
	}

	NodeType Expr(CompilerData* data, CompilerArgs* args) {
		$;
		Node* node = args->cur_node;
		assert(node != NULL);
		NodeType node_type = kUnknownNodeType;
		switch (node->token->type) {
			case kArithOperator:
				if (node->token->val.id == (int)kPlus) {
					node_type = ExprCasePlus(data, args);
				} else {
					assert(0);
				}
				break;
			case kIntNum:
				print::PushConst(data, args, node->token->val.int_val);
				node_type = kIntNodeType;
				break;
			case kFloatNum:
				print::PushConst(data, args, node->token->val.float_val);
				node_type = kFloatNodeType;
				break;
			case kVar:
				node_type = ExprCaseVar(data, args);
				break;
			case kFunc:
				node_type = FuncCall(data, args, true);
				break;
			default:
				printf("token = %d\n", node->token->type);
				assert(0);
		}
		RETURN node_type;
	}

	void StatementCaseVar(CompilerData* data, CompilerArgs* args) {
		$;
		Node* node = args->cur_node;
		assert(node != NULL);
		assert(node->children->sz == 1);

  	Node* child = GetChild(node, 0);
  	CompilerArgs new_args = MakeNewArgs(args, child);
  	NodeType node_type = compile::Expr(data, &new_args);
  	if (node_type == kUnknownNodeType) { RETURN; }

		bool is_global = false;
		Var* var = FindVarGlobalOrLocalOrArgument(data, args, node->token, &is_global);
		if (var == NULL) {
	    ProcessCompilationError(data, node->token, kNoSuchVarError);
	    RETURN;
		}

		if (!DoTypesMatch(node_type, var)) {
			ProcessCompilationError(data, args->cur_node->token, kVarInitInvalidTypeError);
		} else {
			print::PopToVar(data, args, var, is_global);
		}
		$$;
	}

	void StatementCaseReturn(CompilerData* data, CompilerArgs* args) {
		$;
		Node* node = args->cur_node;
		assert(node->token->type == kReturn);
		assert(node->children->sz <= 1);
		bool is_ok = CheckReturnCorrectness(data, args);
		if (!is_ok) {
			RETURN;
		}

		if (node->children->sz == 1) {
	  	Node* child = GetChild(node, 0);
	  	CompilerArgs new_args = MakeNewArgs(args, child);
	  	NodeType node_type = compile::Expr(data, &new_args);
	  	if (node_type == kUnknownNodeType) { RETURN; }
	  	if (!DoTypesMatch(args->cur_func, node_type)) {
	  		ProcessCompilationError(data, node->token, kInvalidReturnTypeError);
	  	} else {
	  		print::PopToFirstReg(data, args, node_type);
	  	}
	  }

	  print::Leave(data, args);
	  print::Ret(data, args);
		RETURN;
	}

	void Statement(CompilerData* data, CompilerArgs* args) {
		$;
		Node* node = args->cur_node;
		assert(node != NULL);
		Token* prev_token = node->token - 1;
		switch (node->token->type) {
			case kVar:
				if (node->children->sz > 0) {
					StatementCaseVar(data, args);
				}
				break;
			case kReturn:
				*args->is_return_found = true;
				StatementCaseReturn(data, args);
				break;
			case kFunc:
				FuncCall(data, args, false);
				break;
			default:
				assert(0);
		}
		fprintf(data->file, "\n");
		RETURN;
	}

	int Function(CompilerData* data, CompilerArgs* args) {
		$;
		Node* node = args->cur_node;
		assert(node != NULL);
		bool is_return_found = false;
		args->is_return_found = &is_return_found;
		args->cur_func = FindFunction(data->tree->functions,
			                            node->token->val.name);
		assert(args->cur_func != NULL);

		fprintf(data->file, "%s:\n", node->token->val.name);
		fprintf(data->file, "\tpush rbp\n");
		fprintf(data->file, "\tmov rbp, rsp\n");
		int last_var_stack_ofs = 0;
		Vector* vars = args->cur_func->local_vars;
		if (vars->sz > 0) {
			Var* last_var = (Var*)VectorBack(vars, sizeof(Var));
			last_var_stack_ofs = last_var->stack_ofs;
		}
		if (last_var_stack_ofs) {
			fprintf(data->file, "\tsub rsp, %d\n", -last_var_stack_ofs);
		}

		size_t sz = node->children->sz;
		for (size_t i = 0; i < sz; ++i) {
			Node* child = GetChild(node, i);
			CompilerArgs new_args = MakeNewArgs(args, child);
		  compile::Statement(data, &new_args);
		}

	  if (args->cur_func->type != kFuncVoid && !is_return_found) {
	  	data->is_error_occured = true;
	  	PrintCompilationError(data->tree->text, args->cur_node->token, kNoReturnError);
	  }
		RETURN 0;
	}
}

void FreeSmartPtrs(Vector* smart_ptrs) {
	$;
	// assert(smart_ptrs != NULL);
	// void** not_freed_ptr = (void**)smart_ptrs->arr;
	// size_t sz = smart_ptrs->sz;
	// for (size_t i = 0; i < sz; ++i) {
	// 	free(*not_freed_ptr++);
	// }
	// VectorDestroy(smart_ptrs);
	$$;
}

int RunThroughAllFunctions(CompilerData* data) {
	$;
	Node* root = data->tree->root;
	size_t sz = root->children->sz;
	for (size_t i = 0; i < sz; ++i) {
		Node* child = GetChild(root, i);
		Token* token = child->token;
		if (token->type == kFunc) {
			CompilerArgs args = {child, 1, NULL, NULL};
		  CATCH_ERROR(compile::Function(data, &args));
		}
	}
	RETURN 0;
}

int PrintBeginningOfFile(FILE* file) {
	$;
	assert(file != NULL);
	FILE* iolib = fopen("iolib.asm", "r");
	if (iolib == NULL) {
		RETURN kNoIolibFile;
	}
	size_t sz = SizeOfFile("iolib.asm");

	char* temp_buf = (char*)calloc(sz, sizeof(char));
	fread(temp_buf, sizeof(char), sz, iolib);
	fwrite(temp_buf, sizeof(char), sz, file);
	fprintf(file, "\n\n");

	RETURN 0;
}

void PrintSectionData(CompilerData* data) {
	$;
	fprintf(data->file, "section .data\n");
	fprintf(data->file, "num_str: times 20 db 'z'\n");

	Vector* g_vars = data->tree->global_vars;
	for (size_t i = 0; i < g_vars->sz; ++i) {
		Var* var = (Var*)VectorGet(g_vars, i, sizeof(Var));
		fprintf(data->file, "%s: dd 0\n", var->name);
	}

	Vector* consts = data->consts;
	for (size_t i = 0; i < consts->sz; ++i) {
		float val = *(float*)VectorGet(consts, i, sizeof(float));
		fprintf(data->file, "const%lu: dd %f\n", i + 1, val);
	}
	$$;
}

int AddIolibConstants(CompilerData* data) {
	$;
	assert(data != NULL);
	float consts[5] = {10, 0, 1, 1e6, -1};
	size_t sz = sizeof(consts)/sizeof(consts[0]);
	for (size_t i = 0; i < sz; ++i) {
		CATCH_ERROR(VectorPushback(data->consts, consts + i, sizeof(float)));
	}
	RETURN 0;
}

int ConstructData(CompilerData* data, FILE* file,
TreeData* tree) {
	$;
	assert(file != NULL);
	assert(tree != NULL);
	data->tree = tree;
	data->file = file;
	CATCH_ERROR(NewVector(&data->consts, sizeof(float)));
	RETURN 0;
}

int SetStackOffsets(CompilerData* data, CompilerArgs* args) {
	$;
	Node* node = args->cur_node;
	assert(node != NULL);
	if (node->token->type == kVar) {
		Token* prev_token = node->token - 1;
		if (prev_token->type == kDecl) {
			VarType var_type = (VarType)prev_token->val.id;
			if (var_type != kVarInt && var_type != kVarFloat) {
				data->is_error_occured = true;
			  PrintCompilationError(data->tree->text, node->token, kInvalidVarTypeError);
			  RETURN 0;
			}

			bool is_global = false;
			Var* var_ptr = FindVarGlobalOrLocalOrArgument(data, args, node->token, &is_global);
			if (var_ptr != NULL) {
		    ProcessCompilationError(data, node->token, kVarRedeclaration);
		    RETURN 0;
			}

			Var var = {node->token->val.name, var_type, -4};
			Vector* vars = args->cur_func->local_vars;
			if (vars->sz == 0) {
				CATCH_ERROR(VectorPushback(vars, &var, sizeof(Var)));
			} else {
				Var* last_var = (Var*)VectorBack(vars, sizeof(Var));
				var.stack_ofs = last_var->stack_ofs - 4;
				CATCH_ERROR(VectorPushback(vars, &var, sizeof(Var)));
			}
		}
		RETURN 0;
	}

	size_t sz = node->children->sz;
	for (size_t i = 0; i < sz; ++i) {
		Node* child = GetChild(node, i);
		args->cur_node = child;
		CATCH_ERROR(SetStackOffsets(data, args));
	}
	RETURN 0;
}

int SetStackOffsetsToLocalVars(CompilerData* data) {
	$;
	assert(data != NULL);
	Node* root = data->tree->root;
	size_t sz = root->children->sz;
	for (size_t i = 0; i < sz; ++i) {
		Node* child = GetChild(root, i);
		Token* token = child->token;
		if (token->type == kFunc) {
			CompilerArgs args = {};
			args.cur_node = child;
			args.cur_func = FindFunction(data->tree->functions,
				                           child->token->val.name);
			assert(args.cur_func != NULL);
			CATCH_ERROR(SetStackOffsets(data, &args));
		}
	}
	RETURN 0;
}

int CompileTree(TreeData* tree) {
	$;
	assert(tree != NULL);
	assert(tree->root != NULL);

	FILE* file = fopen("a.asm", "w");
	CATCH_ERROR(PrintBeginningOfFile(file));
	CompilerData data = {};
	CATCH_ERROR(ConstructData(&data, file, tree));
	CATCH_ERROR(AddIolibConstants(&data));
	CATCH_ERROR(SetStackOffsetsToLocalVars(&data));
	CATCH_ERROR(RunThroughAllFunctions(&data));

  fprintf(data.file, "_start:\n");
  size_t sz = tree->root->children->sz;
	for (size_t i = 0; i < sz; ++i) {
		Node* child = GetChild(tree->root, i);
	  if (child->token->type == kVar) {
			if (child->children->sz > 0) {
		  	CompilerArgs args = {};
		  	args.cur_node = child;
				compile::StatementCaseVar(&data, &args);
			}
	  } else if (child->token->type != kFunc) {
			assert(0);
		}
	}

	fprintf(data.file, "\tcall Main\n");
	fprintf(data.file, "\tsub rsp, 4\n");
	fprintf(data.file, "\tmov [rsp], eax\n");
	fprintf(data.file, "\tcall exit\n\n");
	PrintSectionData(&data);
	fclose(file);

	RETURN data.is_error_occured ? kCompilationError : 0;
}

int CheckIfMainIsOk(Vector* functions) {
	$;
	assert(functions != NULL);

	bool is_main_ok = false;
	bool is_main_found = false;
	size_t sz = functions->sz;
	for (size_t i = 0; i < sz; ++i) {
		Function* func = (Function*)VectorGet(functions, i, sizeof(Function));
		if (strcmp(func->name, "Main") == 0) {
			is_main_found = true;
			if (func->args_num == 0) {
				if (func->type == kFuncInt) {
					is_main_ok = true;
				} else {
					fprintf(stderr, "compilation error: Main has to return int\n");
				}
			} else {
				fprintf(stderr, "compilation error: Main does not take any arguments\n");
			}
			break;
		}
	}

	if (!is_main_found) {
		fprintf(stderr, "compilation error: no Main function\n");
	}

	RETURN is_main_ok ? 0 : kCompilationError;
}

int Compile(CompileFlags* flags, Vector* smart_ptrs) {
	$;
	assert(flags != NULL);
	assert(smart_ptrs != NULL);

	int err = 0;
	Code code = {};
  CATCH_ERROR(CodeConstruct(&code, flags->file_name, smart_ptrs));
	CATCH_ERROR(Tokenize(&code));
	if (flags->debug) {
		TokenDump(code.tokens, code.text);
	}

	TreeData tree = {};
	CATCH_ERROR(TreeConstruct(&tree, &code, smart_ptrs));
	if (flags->need_tree_dump) {
		dot::TreeDump(tree.root);
	} else if (flags->debug) {
		FunctionsAndGlobalVarsDump(tree.global_vars, tree.functions);
	}

	CATCH_ERROR(CheckIfMainIsOk(tree.functions));

	if (flags->need_asm) {
		CATCH_ERROR(CompileTree(&tree));
	}

	RETURN 0;
}

// section .data
// const1: dd 10.0
// const2: dd 0
// const3: dd 1.0
// const4: dd 1000000.0
// const5: dd -1.0
// num_str: times 20 db 'z'

// stack:
// 4
// 3
// 2
// 1
// 0
// но растет сверху вниз
// запушил => rsp уменьшился

// b Compiler.c:




	// void ReturnVar(CompilerData* data, CompilerArgs* args,
	// Node* child) {
	// 	$;
	// 	bool is_global = false;
	// 	Var* var = FindVar(data->tree->global_vars, child->token->val.name);
	// 	if (var != NULL) {
	// 		is_global = true;
	// 	} else {
	// 		Var* var = FindVar(args->cur_func->local_vars, child->token->val.name);
	// 		if (var == NULL) {
	// 	    data->is_error_occured = true;
	// 	    PrintCompilationError(data->tree->text, child->token, kNoSuchVarError);
	// 	    RETURN;
	// 		}
	// 	}

	// 	if (!DoTypesMatch(args->cur_func->type, var->type)) {
	// 	  data->is_error_occured = true;
	// 	  PrintCompilationError(data->tree->text, args->cur_node->token, kInvalidReturnTypeError);
	// 	  RETURN;
	// 	}
	// 	print::Tabs(data->file, args->tabs_num);
	// 	if (args->cur_func->type == kFuncFloat) {
	// 	  if (is_global) {
	// 	  	fprintf(data->file, "movss xmm0, [%s]\n", var->name);
	// 	  } else {
	// 	  	fprintf(data->file, "movss xmm0, [rbp + %d]\n", var->stack_ofs);
	// 	  }
	// 	} else {
	// 	  if (is_global) {
	// 	  	fprintf(data->file, "mov eax, [%s]\n", var->name);
	// 	  } else {
	// 	  	fprintf(data->file, "mov eax, [rbp + %d]\n", var->stack_ofs);
	// 	  }
	// 	}
	// 	$$;
	// }

	// void ReturnFloat(CompilerData* data, CompilerArgs* args) {
	// 	$;
	// 	if (args->cur_func->type != kFuncFloat) {
	// 	  data->is_error_occured = true;
	// 	  PrintCompilationError(data->tree->text, args->cur_node->token, kInvalidReturnTypeError);
	// 	} else {
	// 	  print::PopFloat(data, args, kXmm0);
	// 		print::Leave(data, args);
	// 		print::Ret(data, args);
	// 	}
	// 	$$;
	// }

	// void ReturnInt(CompilerData* data, CompilerArgs* args) {
	// 	$;
	// 	if (args->cur_func->type != kFuncInt) {
	// 	  data->is_error_occured = true;
	// 	  PrintCompilationError(data->tree->text, args->cur_node->token, kInvalidReturnTypeError);
	// 	} else {
	// 	  print::PopInt(data, args, kEax);
	// 		print::Leave(data, args);
	// 		print::Ret(data, args);
	// 	}
	// 	$$;
	// }