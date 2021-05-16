#include "main.h"
struct CompileFlags {
	bool need_tree_dump = false;
	bool debug = false;
	bool need_asm = false;
	bool need_elf = false;
	bool need_help = false;
	const char* file_name = NULL;
};
void FreeSmartPtrs(Vector* smart_ptrs);
int Compile(CompileFlags* flags, Vector* smart_ptrs);