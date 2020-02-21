#ifndef _ALPHAZ_CONSOLE_H_
#define _ALPHAZ_CONSOLE_H_

#include <alphaz/type.h>

void console_init(void);
void clear_screen(void);
ssize_t console_write(const char *, size_t, unsigned char);

#endif
