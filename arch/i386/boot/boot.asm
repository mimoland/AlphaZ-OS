; mbr引导分区



	org 7c00h		; 指示程序加载到7c00h处

BaseOfStack	equ	7c00h	; 栈的段地址
BaseOfScreen	equ	0b800h	; 显存的段地址
RowOfScreen	equ	25	; 屏幕行数
ColOfScreen	equ	80	; 屏幕列数

	jmp short _start	; 占用两个字节
	nop			; 为了满足fat12磁盘的格式，使用nop填充

	; 其中包括fat12磁盘头以及计算后的各部分的偏移位置
	%include "fat12header.inc"

_start:
	; 初始化栈和寄存器
	mov 	ax, cs
	mov	ds, ax
	mov 	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack
	mov	ax, BaseOfScreen
	mov	gs, ax

	call	clear_screen
	mov	ax, 0x0000
	mov	cx, BootMessage

	call	disp_str
	hlt			; 停机


; 清屏
; 参数：无参数
clear_screen:
	push	cx
	push	di
	mov	cx, 1999
	mov	di, 0
clear_screen_loop:
	mov	byte [gs:si], 0x20  	; 写空格
	inc	si
	mov	byte [gs:si], 0x02	; 黑底绿字
	inc	si
	loop	clear_screen_loop
	pop	di
	pop	cx
	ret


; 在屏幕上显示字符串
; 参数： ah=屏幕第几行 al=屏幕第几列 cx=字符串首地址
disp_str:
	; 暂存寄存器
	push 	si
	push 	di

	; 算出屏幕输出的偏移地址
	push	ax
	mov	al, ah
	mov	ah, ColOfScreen
	shl	ah, 1			; 乘2
	mul	ah
	mov	di, ax
	pop	ax
	xor	ah, ah
	shl	al, 1
	add	di, ax

	mov 	si, cx

_disp:	mov 	al, [ds:si]
	cmp	al, 0
	jz	_disp_end
	mov 	[gs:di], al
	inc	di
	mov	byte [gs:di], 0x02	; 黑底绿字
	inc	di
	inc	si
	jmp	_disp
_disp_end:
	pop 	di
	pop	si
	ret

; 字符串统一以0结尾表示结束
BootMessage:
	db 'Booting....',0x0

times	510-($-$$) db 0
	dw	0xaa55
