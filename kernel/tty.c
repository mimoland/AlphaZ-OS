#include <alphaz/type.h>
#include <alphaz/console.h>
#include <alphaz/kernel.h>
#include <alphaz/stdio.h>
#include <alphaz/unistd.h>
#include <alphaz/keyboard.h>
#include <alphaz/string.h>
#include <alphaz/bugs.h>

#include <asm/console.h>
#include <asm/bug.h>
#include <asm/irq.h>

#define COMMAND_BUF_SIZE 128

struct command_buf {
    char buf[COMMAND_BUF_SIZE];
    int tail;
};

struct command {
    char name[128];
    int (*func)(int argc, char *argv[]);
};

#define SHELL_SIZE  32

static struct command shells[SHELL_SIZE];

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
}

static void print_prefix(void)
{
    static char cwdbuf[512];
    int len;

    len = getcwd(cwdbuf, 511);
    cwdbuf[len] = 0;
    printf("[%s]#", cwdbuf);
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
 * tty_task - tty任务，负责读取键盘缓冲区，将字符输出到屏幕
 */
void tty_task(void)
{
    char code;
    int len;
    struct command_buf buf;

    buf.tail = 0;
    chdir("/user/root");
    print_info();
    print_prefix();

    while (1) {
        loop:
        len = read(STDIN_FILENO, &code, 1);
        if (!len || len == -1)
            goto loop;

        if (code == '\n') {
            printf("\n");
            command_exec(&buf);
            print_prefix();
        } else {
            printf("%c", code);
            command_add(&buf, code);
        }
    }
}
