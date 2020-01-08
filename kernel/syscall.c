#include <alphaz/syscall.h>
#include <alphaz/sched.h>
#include <asm/bug.h>

syscall syscall_table[SYS_CALL_SIZE];


void sys_get_ticks(void)
{
    struct syscall_args_struct args;
    struct task_struct * task = current();
    struct thread_struct * thread = get_thread_info(task);
    get_syscall_args(&args, thread);

    args.arg0 = ticks;

    set_syscall_args(&args, thread);
}


void sys_syscall_test(void)
{
    disp_str("*");
}


/**
 * 初始化系统调用表
 */
static inline void setup_syscall_table(void)
{
    syscall_table[0] = sys_get_ticks;
    syscall_table[1] = sys_syscall_test;
}


void syscall_init(void)
{
    setup_syscall_table();
}
