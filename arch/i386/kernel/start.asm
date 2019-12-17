
global _start
extern	asm_start

[bits 32]
_start:
	call	asm_start
	hlt

