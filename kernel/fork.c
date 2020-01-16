#include <alphaz/fork.h>
#include <alphaz/sched.h>
#include <alphaz/syscall.h>
#include <alphaz/kernel.h>
#include <alphaz/malloc.h>


int generate_pid(void)
{
    static int pid = 1;
    return pid++;
}


void sys_fork(void)
{
    struct task_struct *curr, *new;
    struct pt_regs *regs;
    struct syscall_args_struct args;

    curr = current();
    regs = get_pt_regs(curr);
    get_syscall_args(&args, regs);

    new = alloc_page(0, 1);
    new->stack = alloc_page(0, 1);             /* 分配用户栈 */
    new->pid = generate_pid();
    memcpy(new->stack, curr->stack, USER_STACK_SIZE);

    copy_process(new, regs);

    list_add_tail(&new->task, &task_head);

    args.arg0 = new->pid;
    set_syscall_args(&args, regs);
}
