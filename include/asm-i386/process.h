#ifndef _ASM_PROC_H_
#define _ASM_PROC_H_

#include <alphaz/type.h>
#include <asm/const.h>
#include <asm/protect.h>

void init_process();

extern void restart();

/* Number of tasks */
#define NR_TASKS	1

/* stacks of tasks */
#define STACK_SIZE_TESTA	0x8000

#define STACK_SIZE_TOTAL	STACK_SIZE_TESTA

/* 进程退出时用于保存寄存器的状态 */
typedef struct s_stackframe {
	u32	gs;
	u32	fs;
	u32	es;
	u32	ds;
	u32	edi;
	u32	esi;
	u32	ebp;
	u32	kernel_esp;
	u32	ebx;
	u32	edx;
	u32	ecx;
	u32	eax;
	u32	retaddr;
	u32	eip;
	u32	cs;
	u32	eflags;
	u32	esp;
	u32	ss;
} stack_frame;


/* 进程控制块 */
typedef struct s_proc {
	stack_frame regs;
	u16 ldt_sel;
	Descriptor ldts[LDT_SIZE];
	u32 pid;
	char p_name[16];
}Process;

#endif
