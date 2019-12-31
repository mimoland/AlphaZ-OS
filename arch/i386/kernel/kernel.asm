
global _start
extern  kernel_main
extern	arch_start
extern	exception_handler
extern 	spurious_irq

; 导出中断处理函数
global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
; 对应8259a的外部中断
global  hwint00
global  hwint01
global  hwint02
global  hwint03
global  hwint04
global  hwint05
global  hwint06
global  hwint07
global  hwint08
global  hwint09
global  hwint10
global  hwint11
global  hwint12
global  hwint13
global  hwint14
global  hwint15

[section .bss]
stack_space		resb	2014		; 栈空间
stack_top:					; 栈顶

[section .text]
[bits 32]
_start:
	; 切换栈
	mov	esp, stack_space
	call	arch_start
	jmp	0x8:_next			; 0x8为代码段
_next:

	xor	eax, eax
	mov	ax, 0x20
	ltr	ax

	jmp	kernel_main
	hlt

; 中断和异常 -- 硬件中断
; 主片相关宏
%macro  hwint_master    1
        push    %1
        call    spurious_irq
        add     esp, 4
        hlt
%endmacro
; 从片相关宏
%macro  hwint_slave     1
        push    %1
        call    spurious_irq
        add     esp, 4
        hlt
%endmacro

align   16
hwint00:                ; Interrupt routine for irq 0 (the clock).
	iret

align   16
hwint01:                ; Interrupt routine for irq 1 (keyboard)
        hwint_master    1

align   16
hwint02:                ; Interrupt routine for irq 2 (cascade!)
        hwint_master    2

align   16
hwint03:                ; Interrupt routine for irq 3 (second serial)
        hwint_master    3

align   16
hwint04:                ; Interrupt routine for irq 4 (first serial)
        hwint_master    4

align   16
hwint05:                ; Interrupt routine for irq 5 (XT winchester)
        hwint_master    5

align   16
hwint06:                ; Interrupt routine for irq 6 (floppy)
        hwint_master    6

align   16
hwint07:                ; Interrupt routine for irq 7 (printer)
        hwint_master    7

align   16
hwint08:                ; Interrupt routine for irq 8 (realtime clock).
        hwint_slave     8

align   16
hwint09:                ; Interrupt routine for irq 9 (irq 2 redirected)
        hwint_slave     9

align   16
hwint10:                ; Interrupt routine for irq 10
        hwint_slave     10

align   16
hwint11:                ; Interrupt routine for irq 11
        hwint_slave     11

align   16
hwint12:                ; Interrupt routine for irq 12
        hwint_slave     12

align   16
hwint13:                ; Interrupt routine for irq 13 (FPU exception)
        hwint_slave     13

align   16
hwint14:                ; Interrupt routine for irq 14 (AT winchester)
        hwint_slave     14

align   16
hwint15:                ; Interrupt routine for irq 15
        hwint_slave     15



; 中断和异常 -- 异常
divide_error:
	push	0xffffffff	; no err code
	push	0		; vector_no	= 0
	jmp	exception
single_step_exception:
	push	0xffffffff	; no err code
	push	1		; vector_no	= 1
	jmp	exception
nmi:
	push	0xffffffff	; no err code
	push	2		; vector_no	= 2
	jmp	exception
breakpoint_exception:
	push	0xffffffff	; no err code
	push	3		; vector_no	= 3
	jmp	exception
overflow:
	push	0xffffffff	; no err code
	push	4		; vector_no	= 4
	jmp	exception
bounds_check:
	push	0xffffffff	; no err code
	push	5		; vector_no	= 5
	jmp	exception
inval_opcode:
	push	0xffffffff	; no err code
	push	6		; vector_no	= 6
	jmp	exception
copr_not_available:
	push	0xffffffff	; no err code
	push	7		; vector_no	= 7
	jmp	exception
double_fault:
	push	8		; vector_no	= 8
	jmp	exception
copr_seg_overrun:
	push	0xffffffff	; no err code
	push	9		; vector_no	= 9
	jmp	exception
inval_tss:
	push	10		; vector_no	= A
	jmp	exception
segment_not_present:
	push	11		; vector_no	= B
	jmp	exception
stack_exception:
	push	12		; vector_no	= C
	jmp	exception
general_protection:
	push	13		; vector_no	= D
	jmp	exception
page_fault:
	push	14		; vector_no	= E
	jmp	exception
copr_error:
	push	0xffffffff	; no err code
	push	16		; vector_no	= 10h
	jmp	exception

exception:
	call	exception_handler
	add	esp, 4*2	; 让栈顶指向 EIP，堆栈中从顶向下依次是：EIP、CS、EFLAGS
	hlt

global restart
extern tss
extern  p_proc_ready
restart:
	mov	esp, [p_proc_ready]
	lldt	[esp + P_LDT_SEL]
	lea	eax, [esp + P_STACKTOP]
	mov	dword [tss + TSS3_S_SP0], eax

	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad

	add	esp, 4  ; 跳过retaddr

	iretd		; ring0 -> ring1, 进入TestA


P_STACKBASE	equ	0
GSREG		equ	P_STACKBASE
FSREG		equ	GSREG		+ 4
ESREG		equ	FSREG		+ 4
DSREG		equ	ESREG		+ 4
EDIREG		equ	DSREG		+ 4
ESIREG		equ	EDIREG		+ 4
EBPREG		equ	ESIREG		+ 4
KERNELESPREG	equ	EBPREG		+ 4
EBXREG		equ	KERNELESPREG	+ 4
EDXREG		equ	EBXREG		+ 4
ECXREG		equ	EDXREG		+ 4
EAXREG		equ	ECXREG		+ 4
RETADR		equ	EAXREG		+ 4
EIPREG		equ	RETADR		+ 4
CSREG		equ	EIPREG		+ 4
EFLAGSREG	equ	CSREG		+ 4
ESPREG		equ	EFLAGSREG	+ 4
SSREG		equ	ESPREG		+ 4
P_STACKTOP	equ	SSREG		+ 4
P_LDT_SEL	equ	P_STACKTOP
P_LDT		equ	P_LDT_SEL	+ 4

TSS3_S_SP0	equ	4
