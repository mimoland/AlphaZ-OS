#include <alphaz/type.h>
#include <alphaz/tty.h>
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
