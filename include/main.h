#pragma once
#include "Vector.h"
#include "TextConstruct.h"

static const char* kCompilationResDir = "compilation_res";

enum RuntimeError {
  kNoError,
  kNotEnoughMemory,
  kFileExtensionError,
  kExcessArgsTerminal,
  kNoArgsTerminal,
  kNoCompileFlag,
  kNoInputFileFlag,
  kWrongFileFlagFormat,
  kNoFileInDir,
  kEmptyFile,
  kNoIolibFile,
  kUndefinedSymbols,
  kSyntaxError,
  kCompilationError
};

enum SyntaxError {
  kUnknownSyntaxError,
  kFuncVarDeclExpectedError,
  kFuncVarDeclErrorNameExpected,
  kEqualAfterVarExpectedError,
  kVarDeclError,
  kStatementNewLineExpectedError,
  kVarDeclNewLineExpectedError,
  kCommaParenthExpectedError,
  kVarDeclExpectedError,
  kVarNameExpectedError,
  kBracketOpenExpectedError,
  kBracketCloseExpectedError,
  kParenthOpenExpectedError,
  kParenthCloseExpectedError,
  kStatementExpectedError,
  kExpressionExpectedError,
  kCompSignExpectedError,
  kUnexpectedSymbolError
};

enum CompilationError {
  kUnknownCompilationError,
  kNoReturnError,
  kVoidReturnError,
  kNonVoidReturnError,
  kInvalidReturnTypeError,
  kVarInitInvalidTypeError,
  kNoSuchVarError,
  kNoSuchFuncError,
  kInvalidVarTypeError,
  kDifferentTypeOperandsError,
  kVoidFuncInExpr,
  kIncorrectArgumentsNumToFunc,
  kIncorrecArgumentType,
  kVarRedeclaration
};

enum VarType {
  kVarInt,
  kVarFloat
};

enum FuncType {
  kFuncInt,
  kFuncFloat,
  kFuncVoid
};

enum NumType {
  kNumInt,
  kNumFloat
};

enum DeclType {
  kDeclInt,
  kDeclFloat,
  kDeclVoid
};

enum TokenType {
  kUnknownTokenType, // this means error occured
  kElseIf,
  kIf,
  kElse,
  kWhile,
  kBrackO,   // bracket_open  [
  kBrackC,   // bracket_close ]
  kParenthO, // parantheses_open  (
  kParenthC, // parantheses_close )
  kComp,     // compare sign <, >, != etc.
  kArithOperator,
  kBoolOperator,
  kReturn,
  kVar,
  kDecl,
  kEqual,
  kFunc,
  kComma,
  kIntNum,
  kFloatNum,
  kBreak,
  kContinue,
  kNewLine,
  kExit,
  kScan,
  kPrint
};

enum CompareSigns {
  kLess,
  kLessEqual,
  kGreater,
  kGreaterEqual,
  kEqualEqual,
  kNotEqual
};

enum ArithOperator {
  kPlus,
  kMinus,
  kMul,
  kDiv
};

enum BoolOperator {
  kAnd,
  kOr
};

enum Register {
  kEax,
  kEbx,
  kXmm0,
  kXmm1
};

union TokenVal {
  int id;
  int int_val;
  float float_val;
  char* name;
};

struct Position {
  char* ofs = NULL;
  size_t line = 0;
};

struct Token {
  TokenType type = (TokenType)0;
  TokenVal val = {0};
  Position pos = {};
};

struct Var {
  const char* name = NULL;
  VarType type = (VarType)0;
  int stack_ofs = 0;
};

struct Function {
  const char* name = NULL;
  FuncType type = (FuncType)0;
  size_t args_num = 0;
  Vector* args = NULL;       // Vector of Var
  Vector* local_vars = NULL; // Vector of Var
};

struct Code {
  Text* text = NULL;
  Position pos = {};
  NamesBuf* names_buf = NULL;
  Vector* tokens = NULL;      // Vector of Token
};

struct Node {
  Node* parent = NULL;
  Vector* children = NULL;
  Token* token = NULL;
};

struct TreeData {
  Node* root = NULL;
  Text* text = NULL;
  size_t token_idx = 0;
  Vector* tokens = NULL;      // Vector of Token
  Vector* global_vars = NULL; // Vector of Var
  Vector* functions = NULL;   // Vector of Function
};

struct CompilerData {
  TreeData* tree = NULL;
  FILE* file = NULL;
  Vector* consts = NULL; // Vector of float
  bool is_error_occured = false;
};

enum NodeType {
  kUnknownNodeType,
  kIntNodeType,
  kFloatNodeType
};

struct CompilerArgs {
  Node* cur_node = NULL;
  size_t tabs_num = 0;
  bool* is_return_found = NULL;
  Function* cur_func = NULL;
};