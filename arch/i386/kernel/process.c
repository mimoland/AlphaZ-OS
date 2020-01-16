#include <alphaz/type.h>
#include <alphaz/fork.h>
#include <alphaz/sched.h>
#include <asm/cpu.h>


/**
 * 获取用户栈的esp
 */
static u32 get_esp(struct task_struct *new, u32 esp)
{
    return (((u32)new->stack & (~4095)) | (esp & 4095));
}


int copy_process(struct task_struct *new, struct pt_regs *regs)
{
    struct pt_regs *new_regs;

    new->thread.esp0 = (u32)kernel_stack_top(new);
    // task->thread.cr2
    new_regs = get_pt_regs(new);
    *new_regs = *regs;

    new_regs->eax = 0;
    new_regs->esp = get_esp(new, regs->esp);

    new->thread.eip = (u32)ret_from_fork;
    new->thread.esp = (u32)new_regs;
    return 0;
}
