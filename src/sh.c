#include <stdio.h>
#include <stdbool.h>
int Fermat() {
  int a = 1, b = 1, c = 1;
  const int MAX = 1000;
  while(true) {
    if (a * a * a == b * b * b + c * c * c) {
      return 1;
    }
    ++a;
    if (a > MAX) {
      a = 1;
      ++b;
    }
    if (b > MAX) {
      b = 1;
      ++c;
    }
    if (c > MAX) {
      c = 1;
    }
  }
  return 0;
}

int main() {
  printf("%s %s\n", "Great Fermat's theorem is", (Fermat() ? "false": "true"));
}
