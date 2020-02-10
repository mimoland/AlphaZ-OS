#include <alphaz/init.h>
#include <alphaz/stdio.h>
#include <alphaz/tty.h>
#include <alphaz/unistd.h>
#include <asm/bug.h>
#include <asm/system.h>

int init(void)
{
	while (1) {
		hlt();
	}
	return 0;
}
