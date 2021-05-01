#include "../Qlib.h"

const size_t str_max_len = 100;

#ifdef CATCH_ERROR
	#undef CATCH_ERROR
#endif

#define CATCH_ERROR \
	if (is_error) {   \
		RETURN -1;      \
	}

char str[str_max_len] = {};
char* ofs = NULL;
bool is_error = false;

// double Expr();

// void OfsAdvSimple() {
//   while (*ofs == ' ' || *ofs == '\t') {
//   	++ofs;
//   }
// }

void OfsAdv() {
	++ofs;
  while (*ofs == ' ' || *ofs == '\t') {
  	++ofs;
  }
}

// bool IsNumValidSymbol() {
// 	$;
// 	RETURN *ofs >= '0' && *ofs <= '9' || *ofs == '.';
// }

void PrintSyntaxError(const char* error_message) {
	$;
	printf("Syntax error:\n");
	printf("%s\n", str);
	for (size_t i = 0; i < ofs - str; i++) {
		printf("_");
	}
	printf("^ \n%s\n", error_message);
	$$;
}

// double Num() {
// 	$;
// 	if (!isdigit(*ofs)) {
// 		is_error = true;
// 		PrintSyntaxError("Number expected\n");
// 		RETURN NAN;
// 	}

// 	double val = strtof(ofs, &ofs);
// 	OfsAdvSimple();
// 	RETURN val;
// }

// double Bracket() {
// 	$;
// 	if (*ofs == '(') {
// 		OfsAdv();
// 		double val = Expr();
// 		if (*ofs != ')') {
// 			is_error = true;
// 			PrintSyntaxError("No closing bracket found");
// 			RETURN NAN;
// 		}
// 		OfsAdv();
// 		RETURN val;
// 	} else {
// 		RETURN Num();
// 	}
// }

// double MulDiv() {
// 	$;
// 	double ret_val = Bracket();
// 	CATCH_ERROR;

// 	char op = *ofs;
// 	while (op == '*' || op == '/') {
// 		OfsAdv();
// 		double val = Bracket();
// 		CATCH_ERROR;

// 		switch (op) {
// 			case '*':
// 				ret_val *= val;
// 				break;
// 			case '/':
// 				ret_val /= val;
// 		}

// 		op = *ofs;
// 	}

// 	RETURN ret_val;
// }

// double Expr() {
// 	$;
// 	double ret_val = MulDiv();
// 	CATCH_ERROR;

// 	char op = *ofs;
// 	while (op == '+' || op == '-') {
// 		OfsAdv();
// 		double val = MulDiv();
// 		CATCH_ERROR;

// 		switch (op) {
// 			case '+':
// 				ret_val += val;
// 				break;
// 			case '-':
// 				ret_val -= val;
// 		}

// 		op = *ofs;
// 	}
	
// 	RETURN ret_val;
// }

// double MainRule() {
// 	$;
// 	ofs = str;
// 	double val = Expr();
// 	CATCH_ERROR;

// 	if (*ofs != '\0') {
// 		is_error = true;
// 		PrintSyntaxError("Invalid symbol");
// 		RETURN NAN;
// 	}

// 	RETURN val;
// }

int Digit() {
	$;
	int digit = (int)(*ofs - '0');
	OfsAdv();
	return digit;
}

int AndOperation() {
	$;
	int ret_val = Digit();
	CATCH_ERROR;

	while (ofs[0] == '&' && ofs[1] == '&') {
		++ofs;
		OfsAdv();
		int val = Digit();
		CATCH_ERROR;

		ret_val &= val;
	}
	
	RETURN ret_val;
}

int Expr() {
	$;
	int ret_val = AndOperation();
	CATCH_ERROR;

	while (ofs[0] == '|' && ofs[1] == '|') {
		++ofs;
		OfsAdv();
		int val = AndOperation();
		CATCH_ERROR;

		ret_val |= val;
	}
	
	RETURN ret_val;
}

int MainRule() {
	$;
	ofs = str;
	int val = Expr();
	CATCH_ERROR;

	if (*ofs != '\0') {
		is_error = true;
		PrintSyntaxError("Invalid symbol");
		RETURN -1;
	}

	RETURN val;
}

int main() {
	$;
	signal(SIGSEGV, PrintStackInfoAndExit);

	scanf("%[^\n]s", str);

	int ans = MainRule();
	if (is_error) {
		RETURN 199;
	}

	printf("Answer: %d\n", ans);

	RETURN 0;
}

// int main() {
// 	$;
// 	signal(SIGSEGV, PrintStackInfoAndExit);

// 	scanf("%[^\n]s", str);

// 	double ans = MainRule();
// 	if (is_error) {
// 		RETURN 199;
// 	}

// 	printf("Answer: %lf\n", ans);

// 	RETURN 0;
// }