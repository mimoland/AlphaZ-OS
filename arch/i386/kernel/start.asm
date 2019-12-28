
global _start
extern	arch_start
extern	gdt_ptr
extern	idt_ptr

[section .bss]
stack_space		resb	2014		; 栈空间
stack_top:					; 栈顶

[section .text]
[bits 32]
_start:
	; 切换栈
	mov	esp, stack_space

	sgdt	[gdt_ptr]
	call	arch_start
	lgdt	[gdt_ptr]
	lidt	[idt_ptr]
	jmp	0x8:_next			; 0x8为代码段
_next:

	hlt

