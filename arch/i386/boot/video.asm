;
; 使用VBE(VESA BIOS Extension)对屏幕显示进行设置，当前处在实模式, fs为big real mode的段
; 寄存器
;
; 若能设置成功，那么会在此进入高分辨率模式，并在物理地址0x510处放入VBE的显示起始物理地址

XResolution	equ	1440		; 水平分辨率
YResolution	equ	900		; 垂直分辨率
BitsPerPixel	equ	0x20		; 每像素占用位宽
MemoryMode	equ	6		; 内存模式类型
DirectColorModeInfo	equ	2	; Direct Color模式属性
VbeBufBase	equ	0000h		; 缓存基址
VbeBufOffset	equ	7e00h		; 缓存界限

setup_video:
	; mov	ax, 0x4f02
	; mov	bx, 0x4192
	; int	10h
	; ret
	; 测试设备是否支持VBE, 并获取VbeInfoBlock
	mov	ax, VbeBufBase
	mov	es, ax
	mov	di, VbeBufOffset
	mov	ax, 0x4f00
	int 	10h
	cmp	ax, 0x004f
	jz	_get_video_mode
	mov	cx, UnsupportVBEMessage
	call	real_mode_disp_str
	ret

_get_video_mode:

	mov	ax, VbeBufBase
	mov	es, ax
	mov	si, VbeBufOffset
	mov	eax, dword [es:si + 14]
	mov	esi, eax

	; 先将模式号拷贝到一个地方
	mov	edi, 0x100000	; 1M
.1:	mov	ax, [es:esi]
	add	esi, 2
	mov	word [fs:edi], ax
	add	edi, 2
	cmp	ax, -1
	jz	.2
	jmp	.1
.2:

	; 逐个检查每个模式号
	mov	esi, 0x100000
	push	esi
.3:
	pop	esi
	mov	ax, [fs:esi]
	cmp	ax, -1
	jz 	_check_end
	add	esi, 2
	mov	cx, ax
	mov	dx, ax			; 暂存当前模式号
	mov	ax, VbeBufBase
	mov	es, ax
	mov	di, VbeBufOffset
	mov	ax, 4f01h
	int  	10h
	cmp	al, 4fh
	jnz 	.3			; 出错，继续下一个

	push	esi
	mov	ax, VbeBufBase
	mov	es, ax
	mov	si, VbeBufOffset

	cmp	word [es:si + 18], XResolution
	jnz	.3
	cmp	word [es:si + 20], YResolution
	jnz	.3
	cmp	byte [es:si + 25], BitsPerPixel
	jnz 	.3
	cmp	byte [es:si + 27], MemoryMode
	jnz	.3
	cmp	byte [es:si + 39], DirectColorModeInfo
	jnz	.3

	; 符合条件
	mov	word [vbemode], dx	; 保存模式号
	mov	eax, dword [es:si + 40]	; 保存起始物理地址
	mov	dword [vbemodeaddr], eax
	jmp	.3

_check_end:
	mov	ax, word [vbemode]
	mov	ebx, dword [vbemodeaddr]
	mov	dx, 0x0000
	mov	es, dx
	mov	di, 0x0510
	mov	dword [es:di], ebx
	mov	bx, 0x4000
	or	bx, ax
	mov	ax, 0x4f02
	int 	10h
	ret


vbemode:	dw 	0
vbemodeaddr:	dd 	0
UnsupportVBEMessage: db 'unsupport vbe',0
