
global _start
extern	arch_start

[bits 32]
_start:
	call	arch_start
	hlt

