#include <alphaz/bugs.h>
#include <alphaz/fork.h>
#include <alphaz/gfp.h>
#include <alphaz/slab.h>
#include <alphaz/kernel.h>
#include <alphaz/linkage.h>
#include <alphaz/malloc.h>
#include <alphaz/sched.h>
#include <alphaz/type.h>
#include <asm/cpu.h>
#include <asm/irq.h>
#include <asm/process.h>

/**
 * 获取用户栈的esp
 */
static u32 get_esp(struct task_struct *new, u32 esp)
{
    return (((u32) new->stack & (~(USER_STACK_SIZE - 1))) |
            (esp & (USER_STACK_SIZE - 1)));
}

int copy_flags(struct task_struct *new, int flags)
{
    new->flags = current->flags;
    return 0;
}

int copy_process(struct task_struct *new, struct pt_regs *regs)
{
	struct pt_regs *new_regs;

	new->thread.esp0 = (unsigned long)kernel_stack_top(new);

	new_regs = get_pt_regs(new);
	memcpy(new_regs, regs, sizeof(struct pt_regs));

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

static int cpoy_mm(struct task_struct *new, int flags)
{
    new->stack = (void *)get_zeroed_page(GFP_HIGHUSER);
    if (!new->stack)
        return -1;
    if (!(new->flags & PF_KERNEL))
		memcpy(new->stack, current->stack, USER_STACK_SIZE);
    new->mm = NULL;
    return 0;
}

static int copy_files(struct task_struct *new, int flags)
{
    struct files_struct *files;

    files = (struct files_struct *)kmalloc(sizeof(struct files_struct), 0);
    assert(files != 0);
    if (!files)
        return -1;
    memcpy(files, current->files, sizeof(struct files_struct));
    new->files = files;
    return 0;
}

pid_t do_fork(struct pt_regs *regs, int flags, unsigned long stack_start,
			unsigned long stack_size)
{
	struct task_struct *new;

	new = (struct task_struct *)get_zeroed_page(GFP_KERNEL);
    if (!new)
        return -1;
    memset(new, 0, sizeof(struct task_struct));
    new->state = TASK_RUNNING;
    new->pid = ++global_pid;
	new->prio = 1;
    new->counter = 1;
    new->alarm = 0;
	new->parent = current;
    new->signal = 0;
    new->cwd = current->cwd;

	list_head_init(&new->children);
	if (copy_flags(new, flags)) {
        panic("copy flags error\n");
        goto do_fork_failed;
    }

    if (cpoy_mm(new, flags)) {
        panic("copy mm error\n");
        goto do_fork_failed;
    }

    if(copy_process(new, regs)) {
        panic("copy process error\n");
        goto do_fork_failed;
    }

    if (copy_files(new, flags)) {
        panic("copy files error\n");
        goto do_fork_failed;
    }
    cli();
    list_add(&new->sibling, &current->children);
	list_add_tail(&new->task, &task_head);
    sti();
	return new->pid;
do_fork_failed:
    return -1;
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
