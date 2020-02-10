#ifndef _ALPHAZ_FORK_H_
#define _ALPHAZ_FORK_H_

#include <alphaz/sched.h>
#include <alphaz/linkage.h>
#include <asm/cpu.h>

#define CLONE_VM        (1 << 0)
#define CLONE_FS        (1 << 1)
#define CLONE_PTRACE    (1 << 2)
#define CLONE_VFORK     (1 << 3)
#define CLONE_STOPPEN   (1 << 4)
#define CLONE_SIGHAND   (1 << 5)
#define CLOSE_THREAD    (1 << 6)

extern pid_t global_pid;

extern void ret_from_fork(void);
extern void kernel_thread_ret(void);

pid_t kernel_thread(int (*fn)(void), void *, unsigned long);

asmlinkage pid_t sys_fork(void);

#endif
