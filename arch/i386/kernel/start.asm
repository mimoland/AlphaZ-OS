org	0h

_start:
	mov	ax, 0xb800
	mov	gs, ax
	mov	ah, 0x0f
	mov	al, 'K'
	mov	[gs:80], ax		; 第一行显示一个字符

	hlt

	times 1024 db 0
