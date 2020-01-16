/*
 * 内核初始化入口
 */

#include <alphaz/sched.h>
#include <alphaz/mm.h>
#include <alphaz/tty.h>
#include <alphaz/stdio.h>
#include <alphaz/syscall.h>
#include <alphaz/kernel.h>
#include <alphaz/keyboard.h>

#include <asm/system.h>
#include <asm/bug.h>
#include <asm/cpu.h>
#include <asm/irq.h>
#include <asm/tty.h>


void kernel_main()
{
    cpu_init();
    irq_init();
    syscall_init();
    mm_init();
    keyboard_init();
    task_init();

    /**
     * 切换到第进程0执行
     */
    sti();
    move_to_user_mode(idle->thread.esp);

    if (!fork()) {
        TestB();
    }

    if (!fork()) {
        TestB();
    }

    TestA();
}
