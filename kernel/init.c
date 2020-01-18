#include <alphaz/init.h>
#include <alphaz/syscall.h>
#include <alphaz/tty.h>
#include <asm/bug.h>

void init(void)
{

    if (!fork()) {
        tty_task();
    }

    while (1) {
        delay(1);
    }
}
