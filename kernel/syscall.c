#include <alphaz/syscall.h>
#include <asm/bug.h>

syscall syscall_table[SYS_CALL_SIZE];


void sys_get_ticks(void)
{
    disp_str("+");
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
