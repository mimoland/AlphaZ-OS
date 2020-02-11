/*
 * 内核初始化入口
 */

#include <alphaz/fork.h>
#include <alphaz/init.h>
#include <alphaz/kernel.h>
#include <alphaz/keyboard.h>
#include <alphaz/mm.h>
#include <alphaz/sched.h>
#include <alphaz/stdio.h>
#include <alphaz/tty.h>
#include <alphaz/unistd.h>

#include <asm/bug.h>
#include <asm/cpu.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
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
	sti();
	kernel_thread(init, NULL, 0);
	while (1) {
		delay(3);
		printk("kernel_main\n");
	}
}
