// #include <stdio.h>
#define size_t long long unsigned int

char ScanOneSymbol() {
	char symbol = 0;
  __asm__ 
  (
      ".intel_syntax noprefix\n"
      "mov rdi, 0\n"
      "mov rsi, rax\n"
      "mov rdx, 1\n"
      "mov rax, 0\n"
      "syscall\n"
      ".att_syntax\n"

      :
      :"rax" (&symbol)
      :"rdi", "rsi", "rdx"
  );
  return symbol;
}

float fscan() {
	bool is_signed = false;
	float int_part = 0;
	char sym = ScanOneSymbol();
	if (sym == '-') {
		is_signed = true;
		sym = ScanOneSymbol();
	}
	while (sym != '.' && sym != '\n') {
		int_part *= 10;
		int_part += sym - '0';
		sym = ScanOneSymbol();
	}

	if (sym == '\n') {
		return is_signed ? -int_part : int_part;
	}

	float float_part = 0;
	float power = 1;
	size_t cnt = 0;
  while ((sym = ScanOneSymbol()) != '\n' && ++cnt <= 6) {
		power *= 10;
		float_part *= 10;
		float_part += sym - '0';
  }
  float_part /= power;

  float res = int_part + float_part;
	return is_signed ? -res : res;
}

int dscan() {
	int res = 0;
	char sym = ScanOneSymbol();
	bool is_signed = false;
	if (sym == '-') {
		is_signed = true;
		sym = ScanOneSymbol();
	}
	while (sym != '\n') {
		res *= 10;
		res += sym - '0';
		sym = ScanOneSymbol();
	}
	return is_signed ? -res : res;
}

char num_str[20] = {};
size_t fprint(float num) {


	char buf[10] = {};
	size_t num_str_ofs = 0;
	bool is_signed = false;
	if (num < 0) {
		is_signed = true;
		num = -num;
		num_str[num_str_ofs++] = '-';
	}

	int int_part = (int)num;
	int float_part = (int)((num - (float)int_part) * 1'000'000);

	size_t ofs = 0;
	while (int_part > 0) {
		buf[ofs++] = int_part % 10;
		int_part /= 10;
	}
	if (ofs == 0) {
		num_str[num_str_ofs++] = '0';
	}
	while (ofs >= 1) {
		num_str[num_str_ofs++] = buf[--ofs] + '0';
	}
	num_str[num_str_ofs++] = '.';

	while (ofs <= 5) {
		buf[ofs++] = float_part % 10;
		float_part /= 10;
	}
	while (ofs >= 1) {
		num_str[num_str_ofs++] = buf[--ofs] + '0';
	}

  // mov  rdi, 1   ; file descriptor
  // mov  rsi, num_str ; buffer
  // mov  rdx, 10;rax ; how many to print
  // mov  rax, 1   ; syscall number
  // syscall
	__asm__ 
  (
      ".intel_syntax noprefix\n"
      "mov rdi, 1\n"
      "mov rsi, %0\n"
      "mov rdx, 11\n"
      "mov rax, 1\n"
      "syscall\n"
      ".att_syntax\n"

      :
      :"r" (num_str)
      :"rdi", "rsi", "rdx", "rax"
  );

  return num_str_ofs;
}

size_t dprint(int num) {
	char buf[10] = {};
	size_t ofs = 0;
	size_t num_str_ofs = 0;
	bool is_signed = false;
	if (num < 0) {
		is_signed = true;
		num = -num;
		num_str[num_str_ofs++] = '-';
	}

	while (num > 0) {
		buf[ofs++] = num % 10;
		num /= 10;
	}
	if (ofs == 0) {
		num_str[num_str_ofs++] = '0';
	}
	while (ofs >= 1) {
		num_str[num_str_ofs++] = buf[--ofs] + '0';
	}

	return num_str_ofs;
}

int main() {
	fprint(fscan());
}