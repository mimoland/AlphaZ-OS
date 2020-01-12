#include <alphaz/keyboard.h>
#include <alphaz/kernel.h>
#include <alphaz/type.h>


struct keyboard_buf kb_in;


void keyboard_handle(u8 code)
{
    if (kb_in.count < KEYBOARD_BUF_SIZE) {
        kb_in.buf[kb_in.head] = code;
        kb_in.head = (kb_in.head + 1) % KEYBOARD_BUF_SIZE;
        kb_in.count++;
    }
    printk("count: %d head: %d tail: %d\n", kb_in.count, kb_in.head, kb_in.tail);
}


ssize_t keyboard_read(char *buf, size_t n)
{
    if (kb_in.count > 0) {
        *buf = kb_in.buf[kb_in.tail];
        kb_in.tail = (kb_in.tail + 1) % KEYBOARD_BUF_SIZE;
        kb_in.count--;
        return 1;
    }
    return 0;
}


/**
 * 键盘处理初始化
 */
void keyboard_init(void)
{
    kb_in.count = kb_in.head = kb_in.tail = 0;
}
