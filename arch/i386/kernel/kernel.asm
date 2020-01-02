
global _start
extern  kernel_main

; 内核栈空间
[section .bss]
stack_space		resb	2048		; 栈空间
stack_top:					; 栈顶

[section .text]
[bits 32]
_start:
	; 切换栈
	mov	esp, stack_space
	jmp	kernel_main
	hlt
