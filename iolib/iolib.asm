global _start
section .text
ScanOneSymbol:
.LFB0:
	dec rsp
	xor rdi, rdi
	mov rsi, rsp
	mov rdx, 1
  xor rax, rax
	syscall
	movzx rax, byte [rsp]
	inc rsp
	ret

fscan:
.LFB1:
	push	rbp
	mov	rbp, rsp
	sub	rsp, 32
	mov	byte [rbp - 26], 0
	pxor	xmm0, xmm0
	movss	dword [rbp - 24], xmm0
	call	ScanOneSymbol
	mov	byte [rbp - 25], al
	cmp	byte [rbp - 25], 45
	jne	.L7
	mov	byte [rbp - 26], 1
	call	ScanOneSymbol
	mov	byte [rbp - 25], al
.L7:
	cmp	byte [rbp - 25], 46
	je	.L6
	cmp	byte [rbp - 25], 10
	je	.L6
	movss	xmm1, dword [rbp - 24]
	movss	xmm0, dword [const1]
	mulss	xmm0, xmm1
	movss	dword [rbp - 24], xmm0
	movsx	eax, byte [rbp - 25]
	sub	eax, 48
	cvtsi2ss	xmm0, eax
	movss	xmm1, dword [rbp - 24]
	addss	xmm0, xmm1
	movss	dword [rbp - 24], xmm0
	call	ScanOneSymbol
	mov	byte [rbp - 25], al
	jmp	.L7
.L6:
	cmp	byte [rbp - 25], 10
	jne	.L8
	cmp	byte [rbp - 26], 0
	je	.L9
	movss	xmm0, dword [rbp - 24]
	movss	xmm1, dword [const2]
	xorps	xmm0, xmm1
	jmp	.L11
.L9:
	movss	xmm0, dword [rbp - 24]
	jmp	.L11
.L8:
	pxor	xmm0, xmm0
	movss	dword [rbp - 20], xmm0
	movss	xmm0, dword [const3]
	movss	dword [rbp - 16], xmm0
	mov	qword [rbp - 8], 0
.L15:
	call	ScanOneSymbol
	mov	byte [rbp - 25], al
	cmp	byte [rbp - 25], 10
	je	.L12
	add	qword [rbp - 8], 1
	cmp	qword [rbp - 8], 6
	ja	.L12
	mov	eax, 1
	jmp	.L13
.L12:
	mov	eax, 0
.L13:
	test	al, al
	je	.L14
	movss	xmm1, dword [rbp - 16]
	movss	xmm0, dword [const1]
	mulss	xmm0, xmm1
	movss	dword [rbp - 16], xmm0
	movss	xmm1, dword [rbp - 20]
	movss	xmm0, dword [const1]
	mulss	xmm0, xmm1
	movss	dword [rbp - 20], xmm0
	movsx	eax, byte [rbp - 25]
	sub	eax, 48
	cvtsi2ss	xmm0, eax
	movss	xmm1, dword [rbp - 20]
	addss	xmm0, xmm1
	movss	dword [rbp - 20], xmm0
	jmp	.L15
.L14:
	movss	xmm0, dword [rbp - 20]
	divss	xmm0, dword [rbp - 16]
	movss	dword [rbp - 20], xmm0
	movss	xmm0, dword [rbp - 24]
	addss	xmm0, dword [rbp - 20]
	movss	dword [rbp - 12], xmm0
	cmp	byte [rbp - 26], 0
	je	.L16
	movss	xmm0, dword [rbp - 12]
	movss xmm1, dword [const5]
	mulss xmm0, xmm1
	; movss	xmm1, dword [const2]
	; xorps	xmm0, xmm1
	jmp	.L11
.L16:
	movss	xmm0, dword [rbp - 12]
.L11:
	leave
	ret

dscan:
.LFB2:
	push	rbp
	mov	rbp, rsp
	sub	rsp, 16
	mov	dword [rbp - 4], 0
	call	ScanOneSymbol
	mov	byte [rbp - 6], al
	mov	byte [rbp - 5], 0
	cmp	byte [rbp - 6], 45
	jne	.L22
	mov	byte [rbp - 5], 1
	call	ScanOneSymbol
	mov	byte [rbp - 6], al
.L22:
	cmp	byte [rbp - 6], 10
	je	.L21
	mov	edx, dword [rbp - 4]
	mov	eax, edx
	sal	eax, 2
	add	eax, edx
	add	eax, eax
	mov	dword [rbp - 4], eax
	movsx	eax, byte [rbp - 6]
	sub	eax, 48
	add	dword [rbp - 4], eax
	call	ScanOneSymbol
	mov	byte [rbp - 6], al
	jmp	.L22
.L21:
	cmp	byte [rbp - 5], 0
	je	.L23
	mov	eax, dword [rbp - 4]
	neg	eax
	jmp	.L25
.L23:
	mov	eax, dword [rbp - 4]
.L25:
	leave
	ret

FPrint:
.LFB3:
	push	rbp
	mov	rbp, rsp
	sub	rsp, 80
	movss	dword [rbp - 68], xmm0
	mov	rax, 40
	mov	qword [rbp - 8], rax
	xor	eax, eax
	mov	qword [rbp - 18], 0
	mov	word [rbp - 10], 0
	mov	qword [rbp - 40], 0
	mov	byte [rbp - 49], 0
	pxor	xmm0, xmm0
	comiss	xmm0, dword [rbp - 68]
	jbe	.L27
	mov	byte [rbp - 49], 1
	movss	xmm0, dword [rbp - 68]
	movss xmm1, dword [const5]
	mulss xmm0, xmm1
	; movss	xmm1, dword [const2]
	; xorps	xmm0, xmm1
	movss	dword [rbp - 68], xmm0
	mov	rax, qword [rbp - 40]
	lea	rdx, [rax + 1]
	mov	qword [rbp - 40], rdx
	lea	rdx, [num_str]
	mov	byte [rax + rdx], 45
.L27:
	movss	xmm0, dword [rbp - 68]
	cvttss2si	eax, xmm0
	mov	dword [rbp - 48], eax
	cvtsi2ss	xmm1, dword [rbp - 48]
	movss	xmm0, dword [rbp - 68]
	subss	xmm0, xmm1
	movaps	xmm1, xmm0
	movss	xmm0, dword [const4]
	mulss	xmm0, xmm1
	cvttss2si	eax, xmm0
	mov	dword [rbp - 44], eax
	mov	qword [rbp - 32], 0
.L30:
	cmp	dword [rbp - 48], 0
	jle	.L29
	mov	ecx, dword [rbp - 48]
	movsx	rax, ecx
	imul	rax, rax, 1717986919
	shr	rax, 32
	mov	edx, eax
	sar	edx, 2
	mov	eax, ecx
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	sal	eax, 2
	add	eax, edx
	add	eax, eax
	sub	ecx, eax
	mov	edx, ecx
	mov	rax, qword [rbp - 32]
	lea	rcx, [rax + 1]
	mov	qword [rbp - 32], rcx
	mov	byte [rbp - 18 + rax], dl
	mov	eax, dword [rbp - 48]
	movsx	rdx, eax
	imul	rdx, rdx, 1717986919
	shr	rdx, 32
	sar	edx, 2
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	mov	dword [rbp - 48], eax
	jmp	.L30
.L29:
	cmp	qword [rbp - 32], 0
	jne	.L33
	mov	rax, qword [rbp - 40]
	lea	rdx, [rax + 1]
	mov	qword [rbp - 40], rdx
	lea	rdx, [num_str]
	mov	byte [rax + rdx], 48
.L33:
	cmp	qword [rbp - 32], 0
	je	.L32
	sub	qword [rbp - 32], 1
	lea	rdx, [rbp - 18]
	mov	rax, qword [rbp - 32]
	add	rax, rdx
	movzx	eax, byte [rax]
	lea	ecx, [rax + 48]
	mov	rax, qword [rbp - 40]
	lea	rdx, [rax + 1]
	mov	qword [rbp - 40], rdx
	lea	rdx, [num_str]
	mov	byte [rax + rdx], cl
	jmp	.L33
.L32:
	mov	rax, qword [rbp - 40]
	lea	rdx, [rax + 1]
	mov	qword [rbp - 40], rdx
	lea	rdx, [num_str]
	mov	byte [rax + rdx], 46
.L35:
	cmp	qword [rbp - 32], 5
	ja	.L34
	mov	ecx, dword [rbp - 44]
	movsx	rax, ecx
	imul	rax, rax, 1717986919
	shr	rax, 32
	mov	edx, eax
	sar	edx, 2
	mov	eax, ecx
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	sal	eax, 2
	add	eax, edx
	add	eax, eax
	sub	ecx, eax
	mov	edx, ecx
	mov	rax, qword [rbp - 32]
	lea	rcx, [rax + 1]
	mov	qword [rbp - 32], rcx
	mov	byte [rbp - 18 + rax], dl
	mov	eax, dword [rbp - 44]
	movsx	rdx, eax
	imul	rdx, rdx, 1717986919
	shr	rdx, 32
	sar	edx, 2
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	mov	dword [rbp - 44], eax
	jmp	.L35
.L34:
	cmp	qword [rbp - 32], 0
	je	.L36
	sub	qword [rbp - 32], 1
	lea	rdx, [rbp - 18]
	mov	rax, qword [rbp - 32]
	add	rax, rdx
	movzx	eax, byte [rax]
	lea	ecx, [rax + 48]
	mov	rax, qword [rbp - 40]
	lea	rdx, [rax + 1]
	mov	qword [rbp - 40], rdx
	lea	rdx, [num_str]
	mov	byte [rax + rdx], cl
	jmp	.L34
.L36:
	mov	rax, qword [rbp - 40]
	mov	rsi, qword [rbp - 8]
	xor	rsi, 40
	je	.L38
.L38:
	leave
	ret

DPrint:
.LFB4:
	push	rbp
	mov	rbp, rsp
	sub	rsp, 64
	mov	dword [rbp - 52], edi
	mov	rax, 40
	mov	qword [rbp - 8], rax
	xor	eax, eax
	mov	qword [rbp - 18], 0
	mov	word [rbp - 10], 0
	mov	qword [rbp - 40], 0
	mov	qword [rbp - 32], 0
	mov	byte [rbp - 41], 0
	cmp	dword [rbp - 52], 0
	jns	.L43
	mov	byte [rbp - 41], 1
	neg	dword [rbp - 52]
	mov	rax, qword [rbp - 32]
	lea	rdx, [rax + 1]
	mov	qword [rbp - 32], rdx
	lea	rdx, [num_str]
	mov	byte [rax + rdx], 45
.L43:
	cmp	dword [rbp - 52], 0
	jle	.L42
	mov	ecx, dword [rbp - 52]
	movsx	rax, ecx
	imul	rax, rax, 1717986919
	shr	rax, 32
	mov	edx, eax
	sar	edx, 2
	mov	eax, ecx
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	sal	eax, 2
	add	eax, edx
	add	eax, eax
	sub	ecx, eax
	mov	edx, ecx
	mov	rax, qword [rbp - 40]
	lea	rcx, [rax + 1]
	mov	qword [rbp - 40], rcx
	mov	byte [rbp - 18+rax], dl
	mov	eax, dword [rbp - 52]
	movsx	rdx, eax
	imul	rdx, rdx, 1717986919
	shr	rdx, 32
	sar	edx, 2
	sar	eax, 31
	sub	edx, eax
	mov	eax, edx
	mov	dword [rbp - 52], eax
	jmp	.L43
.L42:
	cmp	qword [rbp - 40], 0
	jne	.L46
	mov	rax, qword [rbp - 32]
	lea	rdx, [rax + 1]
	mov	qword [rbp - 32], rdx
	lea	rdx, [num_str]
	mov	byte [rax + rdx], 48
.L46:
	cmp	qword [rbp - 40], 0
	je	.L45
	sub	qword [rbp - 40], 1
	lea	rdx, [rbp - 18]
	mov	rax, qword [rbp - 40]
	add	rax, rdx
	movzx	eax, byte [rax]
	lea	ecx, [rax + 48]
	mov	rax, qword [rbp - 32]
	lea	rdx, [rax + 1]
	mov	qword [rbp - 32], rdx
	lea	rdx, [num_str]
	mov	byte [rax + rdx], cl
	jmp	.L46
.L45:
	mov	rax, qword [rbp - 32]
	mov	rsi, qword [rbp - 8]
	xor	rsi, 40
	je	.L48
.L48:
	leave
	ret

fprint:
	movss xmm0, dword [rsp + 8]
	call FPrint
	mov  byte [num_str + rax], 10
	inc  rax
  mov  rdi, 1   ; file descriptor
  mov  rsi, num_str ; buffer
  mov  rdx, rax ; how many to print
  mov  rax, 1   ; syscall number
  syscall
  ret

dprint:
	xor rax, rax
	mov eax, dword [rsp + 8]
	mov rdi, rax
	call DPrint
	mov  byte [num_str + rax], 10
	inc  rax
  mov  rdi, 1   ; file descriptor
  mov  rsi, num_str ; buffer
  mov  rdx, rax ; how many to print
  mov  rax, 1   ; syscall number
  syscall
  ret

exit:
	xor rax, rax
	mov eax, [rsp + 8]
	mov rdi, rax
	mov  rax, 60
	syscall