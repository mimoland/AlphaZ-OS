/*
 * 内核初始化入口
 */

#include <alphaz/sched.h>
#include <alphaz/mm.h>

#include <asm/start.h>
#include <asm/bug.h>
#include <asm/cpu.h>
#include <asm/irq.h>


int kernel_main()
{
    bug_init();
    cpu_init();
    irq_init();
    disp_str("kernel_main\n");
    mm_init();
    hlt();
    return 0;
}
