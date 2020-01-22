#include <alphaz/type.h>
#include <alphaz/tty.h>
#include <alphaz/kernel.h>
#include <alphaz/stdio.h>
#include <alphaz/syscall.h>
#include <alphaz/keyboard.h>
#include <alphaz/string.h>
#include <asm/bug.h>
#include <asm/irq.h>
#include <asm/tty.h>

#define COMMAND_BUF_SIZE 128

struct command_buf {
    char buf[COMMAND_BUF_SIZE];
    int tail;
};

typedef int (*shell_func)(int argc, char *argv[]);

struct shell {
    char name[128];
    shell_func func;
};

#define SHELL_SIZE  32
struct shell shells[SHELL_SIZE];

static int shell_help(int argc, char *argv[])
{
    printf(" shell help:\n");
    printf("  reboot - reboot system\n\n");
    return 0;
}

static int shell_reboot(int argc, char *argv[])
{
    reboot();
    return 0;
}

static void print_info(void)
{
    char *alphaz_info =
"                                                                                "
"================================================================================"
"                                                                                "
"                    Welcome to use AlphaZ OS (develpment)                       "
"                          Copyright(C)  lml 2020                                "
"                                                                                "
"================================================================================"
"                                                                                "
"enter 'help' for more infomation                                                \n";

    char *p;

    for (p = alphaz_info; *p != 0; ++p) {
        printf("%c", *p);
    }
    printf("sh:>");
}


static void command_exec(struct command_buf *buf)
{
    int i;

    if (buf->tail == 0)
        return;
    buf->buf[buf->tail] = 0;

    for (i = 0; i < SHELL_SIZE; i++) {
        if (strcmp(buf->buf, shells[i].name) == 0) {
            shells[i].func(1, NULL);
            goto end;
        }
    }
    printf(" '%s' Bad command or filename\n\n", buf->buf, buf->buf);
    end:
    buf->tail = 0;
}


static void command_add(struct command_buf *buf, char c)
{
    if (buf->tail < COMMAND_BUF_SIZE - 1) {     /* 为\0留一个字节 */
        buf->buf[buf->tail++] = c;
    }
}

void shell_init(void)
{
    // memset(shells, 0, sizeof(shells));

    strcpy(shells[0].name, "help");
    shells[0].func = shell_help;

    strcpy(shells[1].name, "reboot");
    shells[1].func = shell_reboot;
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
                command_exec(&buf);
                printf("sh:>");
            } else {
                printf("%c", code);
                command_add(&buf, code);
            }
        }
    }
}
