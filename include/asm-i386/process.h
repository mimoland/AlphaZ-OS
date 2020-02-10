#ifndef _ASM_PROCESS_H_
#define _ASM_PROCESS_H_

pid_t _kernel_thread(struct pt_regs *, int (*fn)(void), void *, unsigned long);
int copy_process(struct task_struct *, struct pt_regs *);
pid_t do_fork(struct pt_regs *, int, unsigned long, unsigned long);

#endif
