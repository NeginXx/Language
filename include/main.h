#pragma once
#include "Vector.h"
#include "TextConstruct.h"

static const char* kFileExtension = ".neg";

enum RuntimeError {
  kNoError,
  kNotEnoughMemory,
  kFileExtensionError,
  kExcessArgsTerminal,
  kNoArgsTerminal,
  kNoFileInDir,
  kEmptyFile,
  kUndefinedSymbols,
  kSyntaxError
};

enum SyntaxError {
  kFuncVarDecl,
  kNewLineError
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
  kOr,
  kNot
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
  size_t stack_ofs = 0;
};

struct Function {
  const char* name = NULL;
  FuncType type = (FuncType)0;
  size_t args_num = 0;
  Vector* args = NULL;       // Vector of Var
  Vector* local_vars = NULL; // Vector of Var
};

// struct Text {
//   char* buf = NULL;
//   char** lines = NULL;
//   size_t size = 0;
//   size_t lines_num = 0;
// };
struct Code {
  Text* text = NULL;
  Position pos = {};
  NamesBuf* names_buf = NULL;

  Vector* tokens = NULL;      // Vector of Token
  Vector* global_vars = NULL; // Vector of Var
  Vector* functions = NULL;   // Vector of Function
};

struct Node {
  Node* parent = NULL;
  Vector* children = NULL;
  Token* token = NULL;
};