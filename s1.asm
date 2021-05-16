global _start
section .text
ScanOneSymbol:

func:
	push rbp
	mov rbp, rsp
	mov rbx, [rbp + 16]
	leave
	ret

_start:
	push 5
	call func

	xor rax, rax
	mov eax, [rsp + 8]
	mov rdi, rax
	mov  rax, 60
	syscall