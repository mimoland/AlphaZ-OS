; mbr引导分区

	org 7c00h		; 指示程序加载到7c00h处
	mov 	ax, cs
	mov	ds, ax
	mov 	es, ax
	call	disp_str
	hlt			; 停机

disp_str:
	mov	ax, BootMessage
	mov	bp, ax		; es:bp 指向字符串首地址
	mov	cx, [BootMessageLen]
	mov	ax, 1301h	; ah = 13h号功能  al=01h，目标字符串只包含字符，移动光标
	mov	bx, 0002h	; bl = 02h 指定颜色属性，黑底绿字
	mov	dx, 0000h	; 显示在0行0列
	int	10h
	ret

BootMessage:
	db 'Hello world'
BootMessageLen:
	dw (BootMessageLen - BootMessage)

times	510-($-$$) db 0
	dw	0xaa55
