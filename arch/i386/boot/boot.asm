; mbr引导分区



	org 7c00h		; 指示程序加载到7c00h处

BaseOfStack	equ	7c00h	; 栈的段地址
BaseOFLoader	equ	09000h	; loader的段地址
OffsetOfLoader  equ	0100h	; loader的偏移地址
BaseOfScreen	equ	0b800h	; 显存的段地址
RowOfScreen	equ	25	; 屏幕行数
ColOfScreen	equ	80	; 屏幕列数

	jmp short start	; 占用两个字节
	nop			; 为了满足fat12磁盘的格式，使用nop填充

	; 其中包括fat12磁盘头以及计算后的各部分的偏移位置
	%include "fat12header.inc"

start:
	; 初始化栈和寄存器
	mov 	ax, cs
	mov	ds, ax
	mov 	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack
	mov	ax, BaseOfScreen
	mov	gs, ax

	mov	ax, 0x0000
	mov	cx, BootMessage
	call	disp_str

	; 加载loader进内存，首先fat12文件系统上寻找loader

	; 先将软驱复位
	xor	ah, ah
	xor	dl, dl
	int	13h

	; 寻找loader
	mov	word [SectorNo], SectorNoOfRootDirectory	; 要读的扇区号
	mov	word [RootDirSize], RootDirSectors		; 根目录的扇区数
_search_int_root_dir_begin:
	cmp	word [RootDirSize], 0
	jz	_loader_not_found				; 根目录已读完，未找到laoder
	dec 	word [RootDirSize]
	mov 	ax, BaseOFLoader
	mov 	es, ax
	mov 	bx, OffsetOfLoader				; es:bx 指向扇区要读到的地方
	mov 	ax, [SectorNo]					; 要读的扇区号
	mov	cl, 1						; 读一个扇区
	call	read_sector

	mov	si, LoaderFileName				; ds:si 为loader的文件名
	mov	di, OffsetOfLoader				; es:di 为读来的根目录
	cld
	mov	dx, 0x10					; 根目录中一个条目占32字节，一个扇区共16个条目
_search_loader:
	cmp	dx, 0
	jz 	_goto_next_sector				; 未找到，继续下一个扇区
	dec	dx
	mov	cx, 11
_cmp_filename:
	cmp	cx, 0
	jz	_loader_founded
	dec 	cx
	lodsb							; ds:si -> al si+1
	cmp	al, byte [es:di]
	jz 	_cmp_filename_goon				; 继续向下寻找
	jmp	_filename_not_match

_cmp_filename_goon:
	inc	di
	jmp	_cmp_filename


_goto_next_sector:
	add	word [SectorNo], 1
	jmp 	_search_int_root_dir_begin

_filename_not_match:
	and	di, 0xffe0					; 指向本条目开头，这里一个条目为32字节
	add	di, 0x20					; 指向下一个条目
	mov	si, LoaderFileName				; si重新指向loader的文件名
	jmp	_search_loader

_loader_not_found:						; 未找到loader
	mov	ax, 0x0100
	mov	cx, LoaderNotFoundMessage
	call	disp_str
	hlt

_loader_founded:
	mov	ax, 0x0100
	mov	cx, LoaderFoundedMessage
	call	disp_str
	jmp	_load_loader					; 加载loader

	hlt			; 停机

_load_loader:
	; 此时，es:di 所指的便是loader在root dir（根目录）中的表项
	mov 	ax, RootDirSectors
	and 	di, 0xffe0					; 指向当前条目的开始
	add	di, 0x1a					; 指向开始的簇，由于一个扇区一个簇，所以也是扇区号
	mov	cx, word [es:di]
	push	cx
	add	cx, ax
	add	cx, DeltaSectorNo				; loader.bin 的起始扇区号
	mov	ax, BaseOFLoader
	mov	es, ax
	mov	bx, OffsetOfLoader
	mov	ax, cx

_load_loader_goon:

	mov	cl, 1
	call	read_sector
	pop	ax
	call	get_FAT_entry
	cmp	ax, 0xfff
	jz	_loader_loaded
	push	ax
	mov	dx, RootDirSectors
	add	ax, dx
	add	ax, DeltaSectorNo
	add	bx, [BPB_BytsPerSec]
	jmp	_load_loader_goon

_loader_loaded:
	mov	ax, 0x0200
	mov	cx, LoaderLoadedMessage
	call	disp_str

	; 转移到loader去执行
	jmp	BaseOFLoader:OffsetOfLoader


; 找到序号为 ax 的 Sector 在 FAT 中的条目, 结果放在 ax 中
; 参数： ax
; 返回值：ax
get_FAT_entry:
	push	es
	push 	bx
	push	ax

	mov	ax, BaseOFLoader
	sub 	ax, 0x0100
	mov	es, ax
	pop 	ax				; ax恢复原值

	mov	byte [bodd], 0
	mov	bx, 3
	mul	bx 				; dx:ax = bx*3
	mov	bx, 2
	div	bx				; dx:ax / bx  ax=商  dx=余数
	cmp 	dx, 0
	jz	label_even
	mov	byte [bodd], 1
label_even:
	xor 	dx, dx
	mov	bx, [BPB_BytsPerSec]
	div	bx				; ax=fat_entry相对于fat开始处的扇区数
						; dx=fat_entry在扇区内的偏移
	push	dx
	mov 	bx, 0
	add 	ax, SectorNoOfFAT1		; ax=fat_entry所在的扇区号
	mov 	cl, 2				; 读两个扇区
	call	read_sector
	pop	dx
	add	bx, dx
	mov	ax, [es:bx]
	cmp 	byte [bodd], 1
	jnz	label_even_2
	shr	ax, 4
label_even_2:
	and	ax, 0x0fff

	pop	bx
	pop	es
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

; 使用BIOS int 13h 读取软盘扇区函数
; 参数： 从第 ax 个 Sector 开始, 将 cl 个 Sector 读入 es:bx 中
read_sector:
	push	bp
	mov	bp, sp
	sub	esp, 2			; 局部变量空间，保存要读的扇区数

	mov	byte [bp-2], cl
	push	bx
	mov	bl, [BPB_SecPerTrk]	; 每磁道扇区数
	div	bl
	inc	ah
	mov	cl, ah			; 起始扇区号
	mov	dh, al
	shr 	al, 1
	mov 	ch, al			; 柱面号
	and	dh, 1			; 磁头号

	pop	bx
	mov	dl, [BS_DrvNum]		; 驱动器号，0

_read_sector_read:
	mov	ah, 2			; 读功能
	mov	al, byte [bp-2]		; 要读的扇区数
	int	13h
	jc	_read_sector_read	; 如果读取出错CF=1，这时不停地读，直到正确为止

	add	esp, 2			; 平衡栈
	pop	bp
	ret

; 字符串统一以0结尾表示结束
BootMessage: 		db 'Booting...', 0x0
LoaderNotFoundMessage: 	db 'not found', 0x0
LoaderFoundedMessage:	db 'founded', 0x0
LoaderLoadedMessage:	db 'loaded...',0x0

; 存储loader读取过程的相关信息
LoaderFileName: db 'LOADER  BIN'	; loader的文件名，固定长度，注意要大写
RootDirSize:	dw 0			; 根目录占用的扇区数，未初始化
SectorNo:	dw 0			; 要读的扇区号
bodd:		db 0			; 是奇数吗

times	510-($-$$) db 0
	dw	0xaa55
