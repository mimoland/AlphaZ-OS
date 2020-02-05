/*
 * 内核初始化入口
 */

#include <alphaz/sched.h>
#include <alphaz/mm.h>
#include <alphaz/tty.h>
#include <alphaz/stdio.h>
#include <alphaz/unistd.h>
#include <alphaz/kernel.h>
#include <alphaz/keyboard.h>
#include <alphaz/init.h>

#include <asm/system.h>
#include <asm/bug.h>
#include <asm/cpu.h>
#include <asm/irq.h>
#include <asm/tty.h>


void kernel_main()
{
    cpu_init();
    irq_init();
    mm_init();
    keyboard_init();
    shell_init();
    task_init();

    cls_screen();
    /**
     * 切换到第进程0执行
     */
    move_to_user_mode();

    // if (!fork()) {
    //     init();
    // }

    while (1) {
        debug();
        delay(10);
        printf("%d\n", get_ticks());
    }
}
