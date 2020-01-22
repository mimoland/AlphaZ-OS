#ifndef _ALPHAZ_TTY_H_
#define _ALPHAZ_TTY_H_

void shell_init(void);

ssize_t tty_write(const char *buf, size_t n, u8 type);

void tty_task(void);

#endif
