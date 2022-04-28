#include <stdio.h>
#include <math.h>
#include <ctype.h>
const size_t str_max_len = 100;

char str[str_max_len] = {};
char* ofs = NULL;
bool is_error = false;

double Expr();

void OfsAdvSimple() {
  while (*ofs == ' ' || *ofs == '\t') {
  	++ofs;
  }
}

void OfsAdv() {
	++ofs;
  while (*ofs == ' ' || *ofs == '\t') {
  	++ofs;
  }
}

bool IsNumValidSymbol() {
	return *ofs >= '0' && *ofs <= '9' || *ofs == '.';
}

void PrintSyntaxError(const char* error_message) {
	printf("Syntax error:\n");
	printf("%s\n", str);
	for (size_t i = 0; i < ofs - str; i++) {
		printf("_");
	}
	printf("^ \n%s\n", error_message);
}

double Num(bool* is_) {
	if (!isdigit(*ofs)) {
		is_error = true;
		PrintSyntaxError("Number expected\n");
		return NAN;
	}

	double val = strtof(ofs, &ofs);
	OfsAdvSimple();
	return val;
}

double Bracket() {
	;
	if (*ofs == '(') {
		OfsAdv();
		double val = Expr();
		if (*ofs != ')') {
			is_error = true;
			PrintSyntaxError("No closing bracket found");
			return NAN;
		}
		OfsAdv();
		return val;
	} else {
		return Num();
	}
}

double MulDiv() {
	;
	double ret_val = Bracket();
	CATCH_ERROR;

	char op = *ofs;
	while (op == '*' || op == '/') {
		OfsAdv();
		double val = Bracket();
		CATCH_ERROR;

		switch (op) {
			case '*':
				ret_val *= val;
				break;
			case '/':
				ret_val /= val;
		}

		op = *ofs;
	}

	return ret_val;
}

double Expr(bool* is_processed) {
	bool is_processed_here = false;
	double ret_val = MulDiv(&is_processed_here);
	char op = *ofs;
	if (!is_processed && op != '+' && op != '-' && op != '\0') {
		PrintSyntaxError("Expected arithmetic operator");
		return NAN;
	}
	while (op == '+' || op == '-') {
		OfsAdv();
		bool is_processed_here = false;
		double val = MulDiv(&is_processed_here);
		if (!is_processed_here) {
			return NAN;
		}

		switch (op) {
			case '+':
				ret_val += val;
				break;
			case '-':
				ret_val -= val;
		}

		op = *ofs;
	}
	
	return ret_val;
}

double MainRule() {
	ofs = str;
	bool is_processed = false;
	double val = Expr(&is_processed);

	if (!is_processed) {
		// PrintSyntaxError("Invalid symbol");
		return NAN;
	}

	return val;
}

int main() {
	scanf("%[^\n]s", str);
	double ans = MainRule();
	if (is_error) {
		return 199;
	}
	printf("Answer: %lf\n", ans);
	return 0;
}

// int Digit() {
// 	;
// 	int digit = (int)(*ofs - '0');
// 	OfsAdv();
// 	return digit;
// }

// int AndOperation() {
// 	;
// 	int ret_val = Digit();
// 	CATCH_ERROR;

// 	while (ofs[0] == '&' && ofs[1] == '&') {
// 		++ofs;
// 		OfsAdv();
// 		int val = Digit();
// 		CATCH_ERROR;

// 		ret_val &= val;
// 	}
	
// 	return ret_val;
// }

// int Expr() {
// 	;
// 	int ret_val = AndOperation();
// 	CATCH_ERROR;

// 	while (ofs[0] == '|' && ofs[1] == '|') {
// 		++ofs;
// 		OfsAdv();
// 		int val = AndOperation();
// 		CATCH_ERROR;

// 		ret_val |= val;
// 	}
	
// 	return ret_val;
// }

// int MainRule() {
// 	;
// 	ofs = str;
// 	int val = Expr();
// 	CATCH_ERROR;

// 	if (*ofs != '\0') {
// 		is_error = true;
// 		PrintSyntaxError("Invalid symbol");
// 		return -1;
// 	}

// 	return val;
// }

// int main() {
// 	;
// 	signal(SIGSEGV, PrintStackInfoAndExit);

// 	scanf("%[^\n]s", str);

// 	int ans = MainRule();
// 	if (is_error) {
// 		return 199;
// 	}

// 	printf("Answer: %d\n", ans);

// 	return 0;
// }