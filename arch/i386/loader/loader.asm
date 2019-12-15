
org	0100h

; 对loader加载位置的更改务必要更改boot中的加载位置和此处的加载位置
BaseOfLoader		equ	09000h			; loader的段地址
OffsetOfLoader		equ	0100h			; loader的偏移地址
BaseOfLoaderPhyAddr	equ	BaseOfLoader*10h	; loader的物理基址

BaseOfScreen		equ	0b800h			; 显存的段地址
RowOfScreen		equ	25			; 屏幕行数
ColOfScreen		equ	80			; 屏幕列数

BaseOfKernel		equ	08000h			; kernel的段地址
OffsetOfKernel		equ	0h			; kernel的偏移地址

[bits 16]
	jmp	short _start
	nop
	; 这里引用的目的只是使用其中的变量，并不是fat12格式的头
	%include "fat12header.inc"
	%include "pm.inc"

; 段描述符
; 			段基址 段界限， 段属性
desc_null:	Descriptor  	0, 	 0, 		 0				 ; 空描述符
desc_code:	Descriptor 	0, 	 0xfffff, 	DA_CR  |DA_32 | DA_LIMIT_4K  	; 4GB代码段
desc_data_rw:	Descriptor 	0, 	 0xfffff, 	DA_DRW |DA_32 | DA_LIMIT_4K 	; 4GB数据段
desc_video:	Descriptor 	0xb8000, 0xffff,	DA_DRW |DA_DPL3	   		; 显存

; 用于gdtr的加载
gdt_len:	equ	$ - desc_null
gdt_ptr:	dw	gdt_len - 1							; 界限
		dd 	BaseOfLoaderPhyAddr + desc_null					; 基址
; 段选择子
selector_code:		equ	desc_code    - desc_null
selector_data:		equ	desc_data_rw - desc_null
selector_video:		equ	desc_video   - desc_null + SA_RPL3			; RPL为3

_start:
	; 初始化寄存器
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, TopOfStack
	mov	ax, BaseOfScreen
	mov	gs, ax

	; 显示消息
	mov	cx, LoaderComeInMessage
	call	real_mode_disp_str


	; 获取内存大小，供保护模式开启分页使用
	mov	ebx, 0				; ebx = 后续值, 开始时需为 0
	mov	di, MemCheckARDS		; es:di 指向一个地址范围描述符结构(ARDS)
_mem_check_begin:
	mov	eax, 0E820h			; eax = 0000E820h
	mov	ecx, 20				; ecx = 地址范围描述符结构的大小
	mov	edx, 0534D4150h			; edx = 'SMAP'
	int	15h
	jc	_mem_check_fail
	; 累加内存
	mov	eax, [MemSizeTmpLow]
	add	eax, [MemARDSSizeLow]
	mov	[MemSizeTmpLow], eax
	mov	eax, [MemSizeTmpHig]
	adc	eax, [MemARDSSizeHig]
	mov	[MemSizeTmpHig], eax
	; 判断是否检测完
	cmp	ebx, 0
	jne	_mem_check_begin
	jmp	_mem_check_end
_mem_check_fail:
	mov	cx, MemCheckFailMessage
	call	real_mode_disp_str

	hlt

_mem_check_end:
	; 将要使用的内存大小保存到MemSize处，如果内存大于4GB，则只使用4GB
	mov	eax, [MemSizeTmpLow]
	mov	[MemSize], eax
	cmp	dword [MemSizeTmpHig], 0
	jz	_mem_save_end					; 内存不大于则保存结束，否则保存0xffffffff到MemSize处
	mov	eax, 0xffffffff
	mov	[MemSize], eax
_mem_save_end:
	mov	cx, MemCheckEndMessage
	call	real_mode_disp_str


	; 加载内核，和boot中加载loader并无区别
	; 寻找KERNEL.BIN
	mov	word [SectorNo], SectorNoOfRootDirectory	; 要读的扇区号
	mov	word [RootDirSize], RootDirSectors		; 根目录的扇区数
_search_int_root_dir_begin:
	cmp	word [RootDirSize], 0
	jz	_kernel_not_found				; 根目录已读完，未找到kernel
	dec 	word [RootDirSize]
	mov 	ax, BaseOfKernel
	mov 	es, ax
	mov 	bx, OffsetOfKernel				; es:bx 指向扇区要读到的地方
	mov 	ax, [SectorNo]					; 要读的扇区号
	mov	cl, 1						; 读一个扇区
	call	real_mode_read_sector

	mov	si, KernelFileName				; ds:si 为kernel的文件名
	mov	di, OffsetOfKernel				; es:di 为读来的根目录
	cld
	mov	dx, 0x10					; 根目录中一个条目占32字节，一个扇区共16个条目
_search_kernel:
	cmp	dx, 0
	jz 	_goto_next_sector				; 未找到，继续下一个扇区
	dec	dx
	mov	cx, 11
_cmp_filename:
	cmp	cx, 0
	jz	_kernel_founded
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
	mov	si, KernelFileName				; si重新指向kernel的文件名
	jmp	_search_kernel

_kernel_not_found:						; 未找到kernel
	mov	cx, KernelNotFoundMessage
	call	real_mode_disp_str
	hlt

_kernel_founded:
	mov	cx, KernelFoundedMessage
	call	real_mode_disp_str
	jmp	_load_kernel					; 加载kernel

	hlt			; 停机

_load_kernel:
	; 此时，es:di 所指的便是kernel在root dir（根目录）中的表项
	mov 	ax, RootDirSectors
	and 	di, 0xffe0					; 指向当前条目的开始
	add	di, 0x1a					; 指向开始的簇，由于一个扇区一个簇，所以也是扇区号
	mov	cx, word [es:di]
	push	cx
	add	cx, ax
	add	cx, DeltaSectorNo				; kernel.bin 的起始扇区号
	mov	ax, BaseOfKernel
	mov	es, ax
	mov	bx, OffsetOfKernel
	mov	ax, cx

_load_kernel_goon:

	mov	cl, 1
	call	real_mode_read_sector
	pop	ax
	call	real_mode_get_FAT_entry
	cmp	ax, 0xfff
	jz	_kernel_loaded
	push	ax
	add	ax, RootDirSectors
	add	ax, DeltaSectorNo
	add	bx, [BPB_BytsPerSec]
	jmp	_load_kernel_goon

_kernel_loaded:
	; kernel加载完后，先关闭软驱马达
	call	kill_motor
	mov	ax, 0x0200
	mov	cx, KernelLoadedMessage
	call	real_mode_disp_str

	; 开始进入保护模式
	; 加载gdtr
	lgdt	[gdt_ptr]

	cli

	; 打开A20地址线
	in	al, 0x92
	or 	al, 0x02
	out 	0x92, al

	; 打开cr0中的PE位
	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	; 跳入保护模式

	jmp 	dword selector_code:(BaseOfLoaderPhyAddr+protect_mode)

	hlt


; 在屏幕上显示字符串，实模式下使用，字符串以0结尾
; 参数： cx=字符串首地址
real_mode_disp_str:
	pushad
	push 	es
	mov	dh, [RowOfMessageBegin]		; 消息在第几行显示
	xor	dl, dl
	inc	byte [RowOfMessageBegin]
	mov	ax, cs
	mov	es, ax
	mov	bp, cx
	; 计算出字符串的长度
	mov	si, cx
	xor	cx, cx
_next_char:
	cmp	byte [ds:si], 0
	jz	_is_end
	inc	si
	inc	cx
	jmp	_next_char
_is_end:
	mov	bx, 0x0002		; 黑底绿字
	mov	ax, 0x1301
	int	10h
	pop	es
	popad
	ret

; 使用BIOS int 13h 读取软盘扇区函数
; 参数： 从第 ax 个 Sector 开始, 将 cl 个 Sector 读入 es:bx 中
real_mode_read_sector:
	pushad
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
	popad
	ret


; 找到序号为 ax 的 Sector 在 FAT 中的条目, 结果放在 ax 中
; 参数： ax
; 返回值：ax
real_mode_get_FAT_entry:
	push	es
	push 	bx
	push	cx
	push	dx

	push	ax
	mov	ax, BaseOfKernel
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
	call	real_mode_read_sector
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

; 关闭软驱马达
kill_motor:
	pushad
	mov	dx, 0x03f2
	mov	al, 0
	out	dx, al
	popad
	ret


; 一些消息
RowOfMessageBegin:	db	3			; 消息开始的行数
LoaderComeInMessage:	db 	'program had came in the loader now....', 0
KernelNotFoundMessage:	db	'kernel was not found',0
KernelFoundedMessage:	db 	'kernel	was founded',0
KernelLoadedMessage:	db	'kernel	was loaded',0


KernelFileName: db 'KERNEL  BIN'	; loader的文件名，固定长度，注意要大写
RootDirSize:	dw 0			; 根目录占用的扇区数，未初始化
SectorNo:	dw 0			; 要读的扇区号
bodd:		db 0			; 是奇数吗

; 供内存大小检测所使用的内存空间
MemCheckFailMessage:	db	'memary check is fail...', 0
MemCheckEndMessage:	db 	'memary check is end', 0
MemSize:	dd 	0		; 保存最后要使用的内存大小
MemSizeTmpLow:	dd	0		; 实际内存大小的低位
MemSizeTmpHig:	dd	0		; 实际内存大小的高位
MemCheckARDS:	times 32 db 0		; ARDS描述符的存储位置
MemARDSSizeLow	equ	MemCheckARDS + 8	; ARDS中内存大小的低32位
MemARDSSizeHig	equ	MemCheckARDS + 12	; ARDS中内存大小的高32位

; ======================== 以下为保护模式 ==============================

; 将内存进行32位对齐
align	32

[bits 32]

protect_mode:
		mov	ax, selector_data
		mov	ds, ax
		mov	es, ax
		mov	fs, ax
		mov	ss, ax
		mov	esp, TopOfStackProtMode
		mov	ax, selector_video
		mov	gs, ax

		mov	ecx, ComeInProtModeMessage
		call	disp_str

		hlt

; 保护模式下的字符串显示，不能再使用中断
; ecx=要显示的字符串首地址
disp_str:
	pushad
	xor	eax, eax
	mov	al, [RowOfMessageBeginProtMode]
	inc	byte [RowOfMessageBeginProtMode]
	mov	ah, ColOfScreen
	shl	ah, 1
	mul	ah
	mov	edi, eax
	mov	esi, ecx
_disp_str_lable:
	mov	al, [ds:esi]
	cmp	al, 0
	jz	_disp_str_end
	mov	[gs:edi], al
	inc	edi
	mov	byte [gs:edi], 0x02
	inc	edi
	inc 	esi
	jmp	_disp_str_lable
_disp_str_end:
	popad
	ret


; 保护模式中的一些消息, 一定注意进入保护模式后地址的变化
; RowOfMessageBegin 还是使用前面的
RowOfMessageBeginProtMode	equ	RowOfMessageBegin + BaseOfLoaderPhyAddr

_ComeInProtModeMessage:	db  'program had jmped in protect mode now....', 0
ComeInProtModeMessage	equ	_ComeInProtModeMessage + BaseOfLoaderPhyAddr

; 下面开辟一些内存空间供程序使用
BottmOfStack:   times	1024	db	0				; 1k栈空间
TopOfStack 		equ	$					; 实模式的栈顶
TopOfStackProtMode	equ	BaseOfLoaderPhyAddr + TopOfStack 	; 保护模式的栈顶
