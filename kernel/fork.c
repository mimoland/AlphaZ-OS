#include <alphaz/fork.h>
#include <alphaz/sched.h>
#include <alphaz/unistd.h>
#include <alphaz/linkage.h>
#include <alphaz/kernel.h>
#include <alphaz/malloc.h>


int generate_pid(void)
{
    pid = (pid + 1) % 65536;
    return pid;
}


asmlinkage long sys_fork(void)
{
    struct task_struct *curr, *new;
    struct pt_regs *regs;

    curr = current;
    regs = get_pt_regs(curr); /* 待重构 */

    new = alloc_page(0, 1);
    new->state = TASK_RUNNING;
    new->flags = curr->flags;

    new->stack = alloc_page(0, 1);
    new->pid = generate_pid();
    new->counter = curr->counter;
    new->alarm = curr->alarm;

    memcpy(new->stack, curr->stack, USER_STACK_SIZE);

    new->parent = curr;
    list_head_init(&new->children);

    copy_process(new, regs);
    new->mm = NULL;

    new->signal = 0;

    list_add_tail(&new->task, &task_head);

    list_add(&new->sibling, &curr->children);

    return new->pid;
}
