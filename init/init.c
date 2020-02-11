#include <alphaz/fat32.h>
#include <alphaz/init.h>
#include <alphaz/kernel.h>
#include <alphaz/stdio.h>
#include <alphaz/tty.h>
#include <alphaz/unistd.h>

#include <asm/bug.h>
#include <asm/disk.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>

int init(void)
{
	disk_init();
	fat32_init();

	while (1) {
		delay(3);
		printk("init\n");
	}
	return 0;
}
