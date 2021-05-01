#include <stdio.h>
#define F(0) 1
#define F(1) 1
#define F(i) F(i - 1) + F(i - 2)
int main() {
	printf("%lu\n", F(5));
}
