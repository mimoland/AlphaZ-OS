/*
 * 中断入口
 */

#include <alphaz/linkage.h>

.section .text

#define SAVE_ALL    \
    cld;            \
    pushal;         \
    pushl   %ds;    \
    pushl   %es;    \
    pushl   %fs;    \
    pushl   %gs

#define RESTOR_ALL   \
    popl    %gs;     \
    popl    %fs;     \
    popl    %es;     \
    popl    %ds;     \
    popal

#define INTERRUPT_RETURN     \
    addl    $4, %esp;        \
    iretl

#define PUSH_PT_ARGS    \
    movl    %esp, %eax; \
    pushl   %eax

#define POP_PT_ARGS     \
    addl    $4, %esp

/* could not use stack */
#define RESET_8259A_CHIP    \
    movb    $0x20, %al;     \
    outb    %al, $0x20;     \
    outb    %al, $0xa0

/* could not use stack */
#define CLOSE_8259A_PIN(irq)    \
    movb    $8, %al;            \
    cmpb    $irq, %al;          \
    jl      1f;                 \
    inb     $0x21, %al;         \
    bts     $irq,  %ax;         \
    outb    %al, $0x21;         \
    jmp     2f;                 \
  1:inb     $0xa1, %al;         \
    bts     $(irq - 8), %ax;    \
    outb    %al, $0xa1;         \
  2: ;

/* could not use stack */
#define OPEN_8259A_PIN(irq)     \
    movb    $8, %al;            \
    cmpb    $irq, %al;          \
    jl      1f;                 \
    inb     $0x21, %al;         \
    btr     $irq,  %ax;         \
    outb    %al, $0x21;         \
    jmp     2f;                 \
  1:inb     $0xal, %al;         \
    btr     $(irq - 8), %ax;    \
    outb    %al, $0xa1;         \
  2: ;

#define RING0_HW_IRQ_FREAM(irq)    \
    pushl $irq;                 \
    SAVE_ALL;                   \
    CLOSE_8259A_PIN(irq);       \
    RESET_8259A_CHIP;           \
    sti;                        \
    PUSH_PT_ARGS;               \
    call    do_IRQ;             \
    POP_PT_ARGS;                \
    cli;                        \
    OPEN_8259A_PIN(irq);        \
    RESTOR_ALL;                 \
    INTERRUPT_RETURN;

ENTRY(ret_from_fork)
    cli
    OPEN_8259A_PIN(0)
    RESTOR_ALL
    INTERRUPT_RETURN
END(ret_from_fork)

ENTRY(hwint0x20) RING0_HW_IRQ_FREAM(0x20); END(hwint0x20)
ENTRY(hwint0x21) RING0_HW_IRQ_FREAM(0x21); END(hwint0x21)
ENTRY(hwint0x22) RING0_HW_IRQ_FREAM(0x22); END(hwint0x22)
ENTRY(hwint0x23) RING0_HW_IRQ_FREAM(0x23); END(hwint0x23)
ENTRY(hwint0x24) RING0_HW_IRQ_FREAM(0x24); END(hwint0x24)
ENTRY(hwint0x25) RING0_HW_IRQ_FREAM(0x25); END(hwint0x25)
ENTRY(hwint0x26) RING0_HW_IRQ_FREAM(0x26); END(hwint0x26)
ENTRY(hwint0x27) RING0_HW_IRQ_FREAM(0x27); END(hwint0x27)
ENTRY(hwint0x28) RING0_HW_IRQ_FREAM(0x28); END(hwint0x28)
ENTRY(hwint0x29) RING0_HW_IRQ_FREAM(0x29); END(hwint0x29)
ENTRY(hwint0x2a) RING0_HW_IRQ_FREAM(0x2a); END(hwint0x2a)
ENTRY(hwint0x2b) RING0_HW_IRQ_FREAM(0x2b); END(hwint0x2b)
ENTRY(hwint0x2c) RING0_HW_IRQ_FREAM(0x2c); END(hwint0x2c)
ENTRY(hwint0x2d) RING0_HW_IRQ_FREAM(0x2d); END(hwint0x2d)
ENTRY(hwint0x2e) RING0_HW_IRQ_FREAM(0x2e); END(hwint0x2e)
ENTRY(hwint0x2f) RING0_HW_IRQ_FREAM(0x2f); END(hwint0x2f)

/* 中断和异常 -- 异常 */
ENTRY(divide_error)
	pushl	$0xffffffff
	pushl	$0
	jmp 	exception
END(divide_error)

ENTRY(single_step_exception)
	pushl	$0xffffffff
	pushl	$1
	jmp 	exception
END(single_step_exception)

ENTRY(nmi)
	pushl	$0xffffffff
	pushl	$2
	jmp	    exception
END(nmi)

ENTRY(breakpoint_exception)
	pushl	$0xffffffff
	pushl	$3
	jmp	    exception
END(breakpoint_exception)

ENTRY(overflow)
	pushl	$0xffffffff
	pushl	$4
	jmp	    exception
END(overflow)

ENTRY(bounds_check)
	pushl	$0xffffffff
	pushl	$5
	jmp	    exception
END(bounds_check)

ENTRY(inval_opcode)
	pushl	$0xffffffff
	pushl	$6
	jmp	    exception
END(inval_opcode)

ENTRY(copr_not_available)
	pushl	$0xffffffff
	pushl	$7
	jmp	    exception
END(copr_not_available)

ENTRY(double_fault)
	pushl	$8
	jmp	    exception
END(double_fault)

ENTRY(copr_seg_overrun)
	pushl	$0xffffffff
	pushl	$9
	jmp	    exception
END(copr_seg_overrun)

ENTRY(inval_tss)
	pushl	$10
	jmp	    exception
END(inval_tss)

ENTRY(segment_not_present)
	pushl	$11
	jmp	    exception
END(segment_not_present)

ENTRY(stack_exception)
	pushl	$12
	jmp	    exception
END(stack_exception)

ENTRY(general_protection)
	pushl	$13
	jmp	    exception
END(general_protection)

ENTRY(page_fault)
	pushl	$14
	jmp	    exception
END(page_fault)

ENTRY(copr_error)
	pushl	$0xffffffff
	pushl	$16
	jmp	    exception
END(copr_error)

exception:
    SAVE_ALL
    PUSH_PT_ARGS
    cli
	call	do_IRQ
    hlt

/* 系统调用入口 int 0x80
 * eax中为功能号
 */
ENTRY(sys_call)
    pushl   %eax
	SAVE_ALL
    PUSH_PT_ARGS
	sti
	call	do_IRQ
	cli
	POP_PT_ARGS
    RESTOR_ALL
    INTERRUPT_RETURN
END(sys_call)
