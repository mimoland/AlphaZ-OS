
global _start

[bits 32]
_start:
	mov	ah, 0x0f
	mov	al, 'K'
	mov	[gs:80], ax		; 第一行显示一个字符

	hlt

	times 1024 db 0
