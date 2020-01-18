#include <alphaz/type.h>
#include <alphaz/tty.h>
#include <alphaz/kernel.h>
#include <alphaz/stdio.h>
#include <alphaz/syscall.h>
#include <alphaz/keyboard.h>
#include <asm/bug.h>
#include <asm/irq.h>
#include <asm/tty.h>

#define COMMAND_BUF_SIZE 128

struct command_buf {
    char buf[COMMAND_BUF_SIZE];
    int tail;
};


static void print_info(void)
{
    char *alphaz_info =
"                                                                                "
"================================================================================"
"                                                                                "
"                    Welcome to use AlphaZ OS (develpment)                       "
"                          Copyright(C)  lml 2020                                "
"                                                                                "
"================================================================================\n";

    char *p;

    for (p = alphaz_info; *p != 0; ++p) {
        printf("%c", *p);
    }
    printf("sh:>");
}


static void command_print(struct command_buf *buf)
{
    if (buf->tail == 0)
        return;
    buf->buf[buf->tail] = 0;
    printf("You enter the command: '%s', but no operation\n\n", buf->buf);
    buf->tail = 0;
}


static void command_add(struct command_buf *buf, char c)
{
    if (buf->tail < COMMAND_BUF_SIZE - 1) {     /* 为\0留一个字节 */
        buf->buf[buf->tail++] = c;
    }
}


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
    struct command_buf buf;
    buf.tail = 0;

    print_info();

    while (1) {
        // delay(1);               /*fix me，为什么这里必须延时 */
        if (read(STDIN_FILENO, &code, 1)) {
            if (code == '\n') {
                printf("\n");
                command_print(&buf);
                printf("sh:>");
            } else {
                printf("%c", code);
                command_add(&buf, code);
            }
        }
    }
}
