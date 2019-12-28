#ifndef _ASM_INT_H_
#define _ASM_INT_H_

void init_port();

typedef void (*init_handler) ();

void exception_handler();

/* 中断处理函数 */
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

#endif
