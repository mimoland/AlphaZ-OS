/*
 * 内核初始化入口
 */

#include <alphaz/sched.h>
#include <alphaz/mm.h>
#include <alphaz/syscall.h>

#include <asm/start.h>
#include <asm/bug.h>
#include <asm/cpu.h>
#include <asm/irq.h>


int kernel_main()
{
    bug_init();
    cpu_init();
    irq_init();
    syscall_init();
    disp_str("kernel_main\n");
    mm_init();

    /**
     * 任务初始化完成后进入init进程，该方法不再返回，即调用该方法后下面的东西都不会执行
     * 所以该方法必须放在最后调用
     */
    task_init();

    hlt();
    return 0;
}
