#include <alphaz/fork.h>
#include <alphaz/kernel.h>
#include <alphaz/linkage.h>
#include <alphaz/malloc.h>
#include <alphaz/sched.h>
#include <alphaz/type.h>
#include <alphaz/unistd.h>
#include <asm/process.h>

pid_t global_pid = 0;

asmlinkage int sys_fork(void)
{
	struct pt_regs *regs = (struct pt_regs *)current->thread.esp0 - 1;
	return do_fork(regs, CLONE_FS, 0, 0);
}

int kernel_thread(int (*fn)(void), void *args, unsigned long flags)
{
	struct pt_regs regs;
	return _kernel_thread(&regs, fn, args, flags);
}
