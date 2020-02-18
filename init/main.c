/*
 * 内核初始化入口
 */

#include <alphaz/bugs.h>
#include <alphaz/fat32.h>
#include <alphaz/fork.h>
#include <alphaz/fs.h>
#include <alphaz/kernel.h>
#include <alphaz/keyboard.h>
#include <alphaz/mm.h>
#include <alphaz/sched.h>
#include <alphaz/stdio.h>
#include <alphaz/tty.h>
#include <alphaz/unistd.h>

#include <asm/bug.h>
#include <asm/cpu.h>
#include <asm/disk.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/tty.h>

int init(void)
{
    static char buf[512];
    sti();
    disk_init();
    fat32_init();

    move_to_user_mode();

    if (!fork()) {
        while (1) {
            debug();
            delay(3);
        }
    }

    while (1) {
        debug();
        delay(3);
    }

    return 0;
}

void kernel_main()
{
    cpu_init();
    irq_init();
    mm_init();
    keyboard_init();
    shell_init();
    stdio_init();
    task_init();

    cls_screen();
    kernel_thread(init, NULL, 0);
    sti();
    while (1) {
        hlt();
    }
}
