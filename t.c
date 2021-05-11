#include <stdio.h>
int Factorial(int k, int (*func)(int, int(*)())) {
	if (k <= 1) {
		return 1;
	}
	return k * func(k - 1, func);
}
int main() {
	printf("%d\n", Factorial(5, Factorial));
	return 0;
}