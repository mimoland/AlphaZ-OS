

; 中断和异常的入口
%macro ENTRY 1
global %1
align  16
%1:
%endmacro


%macro SAVE_ALL 0
	sub	esp, 4
	pushad
	push	ds
	push	es
	push	fs
	push	gs
%endmacro

%macro POP_AND_RET  0
	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad
	add	esp, 4
	iretd
%endmacro

extern spurious_irq
; 硬件中断的定义
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

_count:	db 0

extern __schedule
ENTRY hwint00
        SAVE_ALL
	mov	al, 0x20
	out	0x20, al

	cmp	byte [_count], 0
	jnz	_hwint00_end
	inc 	byte [_count]
	sti
	inc	byte [gs:0]

	call	__schedule
	cli
	dec	byte [_count]
	test	eax, eax
	jz	_hwint00_end
	mov	esp, eax
_hwint00_end:
	POP_AND_RET


; Interrupt routine for irq 1 (keyboard)
ENTRY hwint01
        hwint_master    1


; Interrupt routine for irq 2 (cascade!)
ENTRY hwint02
        hwint_master    2


; Interrupt routine for irq 3 (second serial)
ENTRY hwint03
        hwint_master    3


; Interrupt routine for irq 4 (first serial)
ENTRY hwint04
        hwint_master    4


; Interrupt routine for irq 5 (XT winchester)
ENTRY hwint05
        hwint_master    5


; Interrupt routine for irq 6 (floppy)
ENTRY hwint06
        hwint_master    6


; Interrupt routine for irq 7 (printer)
ENTRY hwint07
        hwint_master    7


; Interrupt routine for irq 8 (realtime clock).
ENTRY hwint08
        hwint_slave     8


; Interrupt routine for irq 9 (irq 2 redirected)
ENTRY hwint09
        hwint_slave     9


; Interrupt routine for irq 10
ENTRY hwint10
        hwint_slave     10


; Interrupt routine for irq 11
ENTRY hwint11
        hwint_slave     11


; Interrupt routine for irq 12
ENTRY hwint12
        hwint_slave     12


; Interrupt routine for irq 13 (FPU exception)
ENTRY hwint13
        hwint_slave     13


; Interrupt routine for irq 14 (AT winchester)
ENTRY hwint14
        hwint_slave     14



ENTRY hwint15
        hwint_slave     15



; 中断和异常 -- 异常
ENTRY divide_error
	push	0xffffffff	; no err code
	push	0		; vector_no	= 0
	jmp	exception

ENTRY single_step_exception
	push	0xffffffff	; no err code
	push	1		; vector_no	= 1
	jmp	exception

ENTRY nmi
	push	0xffffffff	; no err code
	push	2		; vector_no	= 2
	jmp	exception

ENTRY breakpoint_exception
	push	0xffffffff	; no err code
	push	3		; vector_no	= 3
	jmp	exception

ENTRY overflow
	push	0xffffffff	; no err code
	push	4		; vector_no	= 4
	jmp	exception

ENTRY bounds_check
	push	0xffffffff	; no err code
	push	5		; vector_no	= 5
	jmp	exception

ENTRY inval_opcode
	push	0xffffffff	; no err code
	push	6		; vector_no	= 6
	jmp	exception

ENTRY copr_not_available
	push	0xffffffff	; no err code
	push	7		; vector_no	= 7
	jmp	exception

ENTRY double_fault
	push	8		; vector_no	= 8
	jmp	exception

ENTRY copr_seg_overrun
	push	0xffffffff	; no err code
	push	9		; vector_no	= 9
	jmp	exception

ENTRY inval_tss
	push	10		; vector_no	= A
	jmp	exception

ENTRY segment_not_present
	push	11		; vector_no	= B
	jmp	exception

ENTRY stack_exception
	push	12		; vector_no	= C
	jmp	exception

ENTRY general_protection
	push	13		; vector_no	= D
	jmp	exception

ENTRY page_fault
	push	14		; vector_no	= E
	jmp	exception

ENTRY copr_error
	push	0xffffffff	; no err code
	push	16		; vector_no	= 10h
	jmp	exception


extern exception_handler
exception:
	call	exception_handler
	add	esp, 4*2	; 让栈顶指向 EIP，堆栈中从顶向下依次是：EIP、CS、EFLAGS
	hlt


ENTRY __switch_to_first_task
	mov 	eax, [esp + 4]
	mov	esp, eax

	POP_AND_RET
