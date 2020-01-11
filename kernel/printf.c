#include <stdarg.h>
#include <alphaz/syscall.h>
#include <alphaz/stdio.h>
#include <alphaz/kernel.h>
#include <alphaz/tty.h>


/**
 * printf - 用户态字符串格式化输出函数
 *
 * 该函数使用write系统调用进行输出，只能用于用户态，不能用于内核态
 */
int printf(const char *fmt, ...)
{
    char buf[128];
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    write(STDOUT_FILENO, buf, i);
    va_end(args);

    return i;
}



/**
 * printk - 内核态字符串格式化输出函数
 *
 * 该函数直接使用tty_write实现，只能用于内核态，不能用于用户态
 */
int printk(const char *fmt, ...)
{
    char buf[128];
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    tty_write(buf, i, 0x0f);
    va_end(args);

    return i;
}
