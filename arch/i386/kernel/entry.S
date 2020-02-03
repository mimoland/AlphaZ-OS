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
    popal;           \
    iretl

#define CLOSE_CHIP_M(irq) \
    inb     $0x21, %al;      \
    orb     $(1<<irq), %al; \
    outb    %al, $0x21

#define CLOSE_CHIP_S(irq) \
    inb     $0xa1, %al;      \
    orb     $(1<<irq), %al; \
    outb    %al, $0xa1

#define OPEN_CHIP_M(irq)  \
    inb     $0x21, %al;      \
    movb    $(1<<irq), %ah;  \
    notb    %ah;         \
    andb    %ah, %al; \
    outb    %al, $0x21

#define OPEN_CHIP_S(irq)  \
    inb     $0xa1, %al;      \
    movb    $(1<<irq), %ah;  \
    notb    %ah;         \
    andb    %ah, %al; \
    outb    %al, $0xa1

#define RESET_CHIP  \
    movb    $0x20, %al; \
    outb    %al, $0x20; \
    outb    %al, $0xa0

#define hwint_master(irq)   \
    pushl   $irq;            \
    call   spurious_irq;   \
    addl    $4, %esp ;       \
    hlt

#define hwint_slave(irq)    \
    pushl   $irq;    \
    call   spurious_irq;    \
    addl    $4, %esp;    \
    hlt

ENTRY(timer_interrupt)
    SAVE_ALL
    CLOSE_CHIP_M(0)
    RESET_CHIP
    sti
    call   do_timer
    cli
    OPEN_CHIP_M(0)
    RESTOR_ALL
END(timer_interrupt)

ENTRY(ret_from_fork)
    cli
    OPEN_CHIP_M(0)
    RESTOR_ALL
END(ret_from_fork)

ENTRY(hwint01)
    SAVE_ALL
    CLOSE_CHIP_M(1)
    RESET_CHIP
    call __keyboard_handle
    OPEN_CHIP_M(1)
    RESTOR_ALL
END(hwint01)

ENTRY(hwint02)
    hwint_master(2)
END(hwint02)

ENTRY(hwint03)
    hwint_master(3)
END(hwint03)

ENTRY(hwint04)
    hwint_master(4)
END(hwint04)

ENTRY(hwint05)
    hwint_master(5)
END(hwint05)

ENTRY(hwint06)
    hwint_master(6)
END(hwint06)

ENTRY(hwint07)
    hwint_master(7)
END(hwint07)


ENTRY(hwint08)
    hwint_slave(8)
END(hwint08)

ENTRY(hwint09)
    hwint_slave(9)
END(hwint09)

ENTRY(hwint10)
    hwint_slave(10)
END(hwint10)

ENTRY(hwint11)
    hwint_slave(11)
END(hwint11)

ENTRY(hwint12)
    hwint_slave(12)
END(hwint12)

ENTRY(hwint13)
    hwint_slave(13)
END(hwint13)

ENTRY(disk_interrupt)
    SAVE_ALL
    CLOSE_CHIP_S(6)
    RESET_CHIP
    sti
    call   disk_handler
    cli
    OPEN_CHIP_S(6)
    RESTOR_ALL
END(disk_interrupt)

ENTRY(hwint15)
    hwint_slave(15)
END(hwint15)

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
	call	exception_handler
	addl	$8, %esp	/* 让栈顶指向 EIP，堆栈中从顶向下依次是：EIP、CS、EFLAGS */
	hlt

/* 系统调用入口 int 0x80
 * eax中为功能号
 */
ENTRY(sys_call)
	SAVE_ALL
	sti
	call	*syscall_table(,%eax, 4)
	cli
	RESTOR_ALL
END(sys_call)
