; mbr引导分区



	org 7e00h		; 指示程序加载到8000h处

StartLBA	equ	2048	; boot分区的起始扇区号
BaseOfStack	equ	7c00h	; 栈的段地址
BaseOFLoader	equ	01000h	; loader的段地址
OffsetOfLoader  equ	0000h	; loader的偏移地址
BaseOfScreen	equ	0b800h	; 显存的段地址
RowOfScreen	equ	25	; 屏幕行数
ColOfScreen	equ	80	; 屏幕列数

	jmp short start		; 占用两个字节
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

	; 清屏
	mov	ax, 0600h
	mov	bx, 0700h
	mov	cx, 0
	mov	dx, 0184fh
	int	10h

	mov	ax, 0x0000
	mov	cx, BootMessage
	call	disp_str

	; 加载loader进内存，首先fat12文件系统上寻找loader

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
	add	ax, RootDirSectors
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
	push	cx
	push	dx

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
	mov 	bx, 0
	add 	ax, SectorNoOfFAT1		; ax=fat_entry所在的扇区号
	mov 	cl, 2				; 读两个扇区
	call	read_sector
	add	bx, dx
	mov	ax, [es:bx]
	cmp 	byte [bodd], 1
	jnz	label_even_2
	shr	ax, 4
label_even_2:
	and	ax, 0x0fff

	pop	dx
	pop	cx
	pop	bx
	pop	es
	ret


; 在屏幕上显示字符串
; 参数： ah=屏幕第几行 al=屏幕第几列 cx=字符串首地址
disp_str:
	pushad
	push 	es
	mov	dx, ax
	mov	ax, cs
	mov	es, ax
	mov	bp, cx
	mov	cx, BootMessageLen
	mov	bx, 0x0002		; 黑底绿字
	mov	ax, 0x1301
	int	10h
	pop	es
	popad
	ret

; 使用BIOS int 13h 读取磁盘扇区函数
; 参数： 从第 ax 个 Sector 开始, 将 cl 个 Sector 读入 es:bx 中
read_sector:
	pushad
	xor	ch, ch
	mov	[SectorAddr + SectorAddrSect], cx
	xor	ecx, ecx
	mov	cx, ax
	add	ecx, StartLBA
	mov	dword [SectorAddr + SectorAddrLBA], ecx
	mov	ax, es
	mov	[SectorAddr + SectorAddrBase], ax
	mov	[SectorAddr + SectorAddrOffset], bx
_reread:
	mov	ah, 0x42	; 读硬盘
	mov	dl, 0x80	; 驱动器号
	mov	si, SectorAddr
	int	0x13
	jc	_reread
	popad
	ret

SectorAddrSize		equ	0
SectorAddrReserve	equ	1
SectorAddrSect		equ	2
SectorAddrOffset	equ	4
SectorAddrBase		equ	6
SectorAddrLBA		equ	8
SectorAddr:
	db 	0x10		; 结构体大小
	db 	0		; 保留
	dw 	0		; 扇区数
	dw 	0		; 保存读取内容的内存偏移地址
	dw 	0		; 保存读取内容的内存段地址
	dd 	0		; LBA扇区号
	dd 	0		; 用于大容量存储设备的读取

BootMessageLen:		equ	9
BootMessage: 		db 'Booting..'
LoaderNotFoundMessage: 	db 'no loader'
LoaderFoundedMessage:	db 'founded  '
LoaderLoadedMessage:	db 'loading..'

; 存储loader读取过程的相关信息
LoaderFileName: db 'LOADER  BIN'	; loader的文件名，固定长度，注意要大写
RootDirSize:	dw 0			; 根目录占用的扇区数，未初始化
SectorNo:	dw 0			; 要读的扇区号
bodd:		db 0			; 是奇数吗

times	510-($-$$) db 0
	dw	0xaa55
