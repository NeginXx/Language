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

static const char* kFileExtension = ".neg";

void PrintHelp() {
	printf("-t dump tree\n");
	printf("-d debug\n");
	printf("-a compile to asm file\n");
	printf("-e compile to executable file\n");
	printf("-o name of file to compile, should be .neg format\n");
	printf("-h help\n");
	printf("asm file will be named a.asm\n");
	printf("executable will be named e.out\n");
	printf("\nexample:\n");
	printf("-o myprogram.neg -t -a\n");
}

int ProcessTerminalArgs(CompileFlags* flags, int argc, const char** argv) {
	$;
	assert(flags != NULL);
	assert(argc >= 1);
	assert(argv != NULL);

	if (argc >= 8) {
		RETURN kExcessArgsTerminal;
	} else if (argc <= 1) {
		RETURN kNoArgsTerminal;
	}
	size_t flags_num = 5;
	const char* flags_labels[flags_num] = {"-t", "-d", "-a", "-e", "-h"};
	bool* flags_pointers[flags_num] =
	                       { &flags->need_tree_dump, &flags->debug,
													 &flags->need_asm, &flags->need_elf,
													 &flags->need_help };

	for (size_t flag = 1; flag < argc; ++flag) {
		if (strcmp(argv[flag], "-o") == 0) {
			if (flag == argc - 1) {
				RETURN kWrongFileFlagFormat;
			}
			flags->file_name = argv[flag + 1];
		} else {
			for (size_t i = 0; i < flags_num; ++i) {
				if (strcmp(argv[flag], flags_labels[i]) == 0) {
					*flags_pointers[i] = true;
				}
			}
		}
	}

	if (flags->file_name == NULL) {
		if (flags->debug    ||
				flags->need_asm ||
				flags->need_elf ||
		    flags->need_tree_dump) {
			RETURN kNoInputFileFlag;
		}
	} else {
		const char* dot = strchr(flags->file_name, '.');
		if (dot == NULL || strcmp(dot, kFileExtension) != 0) {
			RETURN kFileExtensionError;
		}

		if (!flags->need_asm && !flags->need_elf &&
			  !flags->need_tree_dump && !flags->debug) {
			RETURN kNoCompileFlag;
		}
	}

	RETURN 0;
}

void SetDebugFlags(CompileFlags* flags) {
	$;
	CompileFlags flagss = {};
	flagss.need_asm = true;
	// flagss.debug = true;
	// flagss.need_tree_dump = true;
	flagss.file_name = "prog.neg";
	memcpy(flags, &flagss, sizeof(CompileFlags));
	$$;
}

#define CHECK_FOR_ERROR(func)         \
	if ((err = func) != 0) {            \
		PrintErrorInfo((RuntimeError)err);\
		FreeSmartPtrs(&smart_ptrs);       \
		RETURN err;                       \
	}

int main(int argc, const char** argv) {
	$;
	signal(SIGSEGV, PrintStackInfoAndExit);
	signal(SIGABRT, PrintStackInfoAndExit);

	int err = 0;
	Vector smart_ptrs = {};
	CHECK_FOR_ERROR(VectorConstruct(&smart_ptrs, sizeof(void*)));

	CompileFlags flags = {};
	#ifdef DEBUG
		SetDebugFlags(&flags);
	#else
		CHECK_FOR_ERROR(ProcessTerminalArgs(&flags, argc, argv));
	#endif

	if (flags.need_help) { PrintHelp(); }
	if (flags.file_name == NULL) { RETURN 0; }
	CHECK_FOR_ERROR(Compile(&flags, &smart_ptrs));
	FreeSmartPtrs(&smart_ptrs);
	printf("compilation finished successfully\n");

	RETURN 0;
}

#undef CHECK_FOR_ERROR