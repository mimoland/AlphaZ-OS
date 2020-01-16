#ifndef _ALPHAZ_FORK_H_
#define _ALPHAZ_FORK_H_

#include <alphaz/sched.h>
#include <asm/cpu.h>

extern void ret_from_fork(void);

int copy_process(struct task_struct *, struct pt_regs *);

void sys_fork(void);

#endif
