

sum:
	push rbp
	mov rbp, rsp
	mov eax, [rbp + 20]
	sub rsp, 4
	mov [rsp], eax
	mov eax, [rbp + 16]
	sub rsp, 4
	mov [rsp], eax
	xor eax, eax
	add eax, [rsp]
	add rsp, 4
	add eax, [rsp]
	add rsp, 4
	sub rsp, 4
	mov [rsp], eax
	mov eax, [rsp]
	add rsp, 4
	leave
	ret

fsum:
	push rbp
	mov rbp, rsp
	movss xmm0, [rbp + 20]
	sub rsp, 4
	movss [rsp], xmm0
	movss xmm0, [rbp + 16]
	sub rsp, 4
	movss [rsp], xmm0
	pxor xmm0, xmm0
	addss xmm0, [rsp]
	add rsp, 4
	addss xmm0, [rsp]
	add rsp, 4
	sub rsp, 4
	movss [rsp], xmm0
	movss xmm0, [rsp]
	add rsp, 4
	leave
	ret

Main:
	push rbp
	mov rbp, rsp
	mov eax, 4
	sub rsp, 4
	mov [rsp], eax
	mov eax, 3
	sub rsp, 4
	mov [rsp], eax
	call sum
	add rsp, 8
	sub rsp, 4
	mov [rsp], eax
	call dprint
	add rsp, 4

	movss xmm0, [const6]
	sub rsp, 4
	movss [rsp], xmm0
	movss xmm0, [const7]
	sub rsp, 4
	movss [rsp], xmm0
	call fsum
	add rsp, 8
	sub rsp, 4
	movss [rsp], xmm0
	call fprint
	add rsp, 4

	mov eax, 0
	sub rsp, 4
	mov [rsp], eax
	mov eax, [rsp]
	add rsp, 4
	leave
	ret

_start:
	call Main
	sub rsp, 4
	mov [rsp], eax
	call exit

section .data
num_str: times 20 db 'z'
const1: dd 10.000000
const2: dd 0.000000
const3: dd 1.000000
const4: dd 1000000.000000
const5: dd -1.000000
const6: dd 334.140015
const7: dd 5.100000
