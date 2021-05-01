global _start
section .text

_start:
	  mov  rdi, 1
	  mov  rsi, printf_buffer
	  mov  rdx, 4 ; printf_buffer size
	  mov  rax, 1 ; syscall number
	  syscall

  EOprogram:            
	  mov  rax, 60     
	  xor  rdi, rdi    
	  syscall

section .data
printf_buffer: db '345', 10, 0