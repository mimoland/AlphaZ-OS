#ifndef _ASM_TTY_H_
#define _ASM_TTY_H_

#include <alphaz/type.h>

ssize_t __tty_write(const char *buf, size_t n, u8 type);

void cls_screen(void);

#endif
