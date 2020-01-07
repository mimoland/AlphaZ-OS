#ifndef _ASM_INT_H_
#define _ASM_INT_H_

#include <asm/cpu.h>

typedef void (*irq_handler) ();

// struct irq_struct
// {
//     unsigned long state;    /* 中断状态 */
//     irq_handler handle;     /* 中断处理函数 */
//     unsigned long vector;   /* 中断向量 */
// };

struct idtr_struct
{
    u16 len;
    u32 base;
} __attribute__((packed));

/* 中断向量数 */
#define IDT_SIZE    256

/**
 * 中断向量表，定义在arch/i386/kernel/irq.c
*/
extern struct gate_struct idt[];

void irq_init();

void exception_handler();
void spurious_irq(int);

/*
 * 开中断 IF=1
 */
static inline void sti(void)
{
    asm volatile("sti");
}


/*
 * 关中断 IF=0
 */
static inline void cli(void)
{
    asm volatile("cli");
}

/* cpu异常处理函数，定义在arch/../kernel/entry.asm */
extern void divide_error();
extern void single_step_exception();
extern void nmi();
extern void breakpoint_exception();
extern void overflow();
extern void bounds_check();
extern void inval_opcode();
extern void copr_not_available();
extern void double_fault();
extern void copr_seg_overrun();
extern void inval_tss();
extern void segment_not_present();
extern void stack_exception();
extern void general_protection();
extern void page_fault();
extern void copr_error();

/* 异常的中断向量 */
#define	INT_VECTOR_DIVIDE		    0x0
#define	INT_VECTOR_DEBUG		    0x1
#define	INT_VECTOR_NMI			    0x2
#define	INT_VECTOR_BREAKPOINT	    0x3
#define	INT_VECTOR_OVERFLOW		    0x4
#define	INT_VECTOR_BOUNDS		    0x5
#define	INT_VECTOR_INVAL_OP		    0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT		0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT		    0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
#define	INT_VECTOR_COPROC_ERR		0x10

/* 硬件中断的处理函数，对应8259a的15个引脚 */
extern void hwint00();
extern void hwint01();
extern void hwint02();
extern void hwint03();
extern void hwint04();
extern void hwint05();
extern void hwint06();
extern void hwint07();
extern void hwint08();
extern void hwint09();
extern void hwint10();
extern void hwint11();
extern void hwint12();
extern void hwint13();
extern void hwint14();
extern void hwint15();


/* 系统调用号 */
#define INT_VECTOR_SYSCALL          0x80
/* defined in entry.asm */
extern void sys_call();

#endif
