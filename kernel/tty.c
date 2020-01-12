#include <alphaz/type.h>
#include <alphaz/tty.h>
#include <alphaz/kernel.h>
#include <alphaz/stdio.h>
#include <alphaz/syscall.h>
#include <alphaz/keyboard.h>
#include <asm/bug.h>
#include <asm/irq.h>
#include <asm/tty.h>


/**
 * tty_write - tty写
 * @buf:    要写的数据的缓冲区
 * @n:      数据的字节长度
 * @type:   字符颜色属性
 */
ssize_t tty_write(const char *buf, size_t n, u8 type)
{
    n = __tty_write(buf, n, type);
    return n;
}


/**
 * tty_task - tty任务，负责读取键盘缓冲区，将字符输出到屏幕
 */
void tty_task(void)
{
    u8 code;
    while (1) {
        delay(1);               /*fix me，为什么这里必须延时 */
        if (read(STDIN_FILENO, &code, 1))
            printf("%c", code);
    }
}
