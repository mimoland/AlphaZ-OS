#ifndef _ALPHAZ_KEYBOARD_H_
#define _ALPHAZ_KEYBOARD_H_

#include <alphaz/type.h>

#define KEYBOARD_BUF_SIZE   128

struct keyboard_buf {
    u8 buf[KEYBOARD_BUF_SIZE];
    int count;               /* 缓冲区扫描码数 */
    int head, tail;
};


// /* 键盘输入缓冲区，defined in keyboard.c */
extern struct keyboard_buf kb_in;

void keyboard_handle(u8);

ssize_t keyboard_read(char *, size_t);

void keyboard_init(void);

#endif
