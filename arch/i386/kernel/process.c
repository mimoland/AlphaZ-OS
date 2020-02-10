#include <alphaz/fork.h>
#include <alphaz/kernel.h>
#include <alphaz/linkage.h>
#include <alphaz/malloc.h>
#include <alphaz/sched.h>
#include <alphaz/type.h>
#include <asm/cpu.h>
#include <asm/process.h>

/**
 * 获取用户栈的esp
 */
static u32 get_esp(struct task_struct *new, u32 esp)
{
	return (((u32) new->stack &(~(USER_STACK_SIZE - 1))) |
			(esp & (USER_STACK_SIZE - 1)));
}

int copy_process(struct task_struct *new, struct pt_regs *regs)
{
	struct pt_regs *new_regs;

	new->thread.esp0 = (unsigned long)kernel_stack_top(new);
	// task->thread.cr2
	new_regs = get_pt_regs(new);
	*new_regs = *regs;

	new_regs->eax = 0;
	if (new->flags & PF_KERNEL) {
		new_regs->esp = (unsigned long)new_regs;
		new->thread.eip = (unsigned long)kernel_thread_ret;
	} else {
		new_regs->esp = get_esp(new, regs->esp);
		new->thread.eip = (unsigned long)ret_from_fork;
	}
	new->thread.esp = (unsigned long)new_regs;
	return 0;
}

pid_t do_fork(struct pt_regs *regs, int flags, unsigned long stack_start,
			unsigned long stack_size)
{
	struct task_struct *new;

	new = (struct task_struct *)alloc_page(0, 1);
	memcpy(new, current, KERNEL_STACK_SIZE);
	new->state = TASK_RUNNING;
	new->pid = ++global_pid;
	new->stack = alloc_page(0, 1);
    if (!(current->flags & PF_KERNEL))
		memcpy(new->stack, current->stack, USER_STACK_SIZE);
	new->parent = current;
	list_head_init(&new->children);
	copy_process(new, regs);
	new->mm = NULL;
	new->signal = 0;
    list_add(&new->sibling, &current->children);
	list_add_tail(&new->task, &task_head);
	return new->pid;
}

pid_t _kernel_thread(struct pt_regs *regs, int (*fn)(void), void *args,
				   unsigned long flags)
{
	memset(regs, 0, sizeof(struct pt_regs));
	regs->cs = SELECTOR_FLAT_C;
	regs->ds = regs->es = regs->ss = regs->fs = SELECTOR_FLAT_RW;
	regs->gs = SELECTOR_VIDEO;
	regs->eip = (unsigned long)kernel_thread_ret;
	regs->ebx = (unsigned long)fn;
	return do_fork(regs, flags | CLONE_VM, 0, 0);
}
