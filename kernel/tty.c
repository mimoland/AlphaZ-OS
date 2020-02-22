#include <alphaz/type.h>
#include <alphaz/console.h>
#include <alphaz/kernel.h>
#include <alphaz/stdio.h>
#include <alphaz/unistd.h>
#include <alphaz/keyboard.h>
#include <alphaz/spinlock.h>
#include <alphaz/string.h>
#include <alphaz/bugs.h>
#include <alphaz/ctype.h>

#include <asm/console.h>
#include <asm/bug.h>
#include <asm/irq.h>

#define SHELL_BUF_SIZE 256

#define NR_SHELL_COMMAND  32

struct command {
    char name[64];
    int (*func)(int argc, char *argv[]);
    char description[128];
};

extern struct command __buildin_command[NR_SHELL_COMMAND];


struct {
    char buf[SHELL_BUF_SIZE];
    int tail;
} shell_buf;

static void shell_append(char c)
{
    if (shell_buf.tail < SHELL_BUF_SIZE)
        shell_buf.buf[shell_buf.tail++] = c;
}

static void shell_pop(void)
{
    if (shell_buf.tail == 0)
        return;
    shell_buf.tail--;
    if (shell_buf.buf[shell_buf.tail] == '\t')  // 要删除的字符
        printf("\b\b\b\b");
    else
        printf("\b");
}

static int shell_exec(void)
{
    int argc;
    char *argv[32];
    int i, flag, ret = -1;

    if (shell_buf.tail == 0)
        return 0;

    for (argc = 0, i = 0, flag = 1; i < shell_buf.tail; i++) {
        if (isspace(shell_buf.buf[i])) {
            shell_buf.buf[i] = 0;
            flag = 1;
            continue;
        }
        if (flag) {
            flag = 0;
            argv[argc++] = shell_buf.buf + i;
        }
    }
    shell_buf.buf[shell_buf.tail] = 0;

    for (i = 0; i < NR_SHELL_COMMAND; i++) {
        if (!strcmp(argv[0], __buildin_command[i].name)) {
            ret = __buildin_command[i].func(argc, argv);
            break;
        }
    }

    if (i == NR_SHELL_COMMAND) {
        printf(" Bad command: %s\n", argv[0]);
        ret = 0;
    }

    shell_buf.tail = 0;     /* 清空缓冲区 */
    if (ret != 0)
        printf("something was wrong, exit code %d\n", ret);
    return ret;
}

static int sh_help(int argc, char *argv[])
{
    int i;

    for (i = 0; i < NR_SHELL_COMMAND; i++) {
        if (__buildin_command[i].func != NULL) {
            printf(" %s - %s\n", __buildin_command[i].name,
                   __buildin_command[i].description);
        }
    }
    return 0;
}

static int sh_reboot(int argc, char *argv[])
{
    reboot();
    return 0;
}

static int sh_pwd(int argc, char *argv[])
{
    char buf[128];
    int n;

    n = getcwd(buf, 127);
    if (n == -1)
        return -1;
    buf[n] = 0;
    printf("%s\n", buf);
    return 0;
}

static int sh_cd(int argc, char *argv[])
{
    int ret = -1;

    if (argc > 2)
        return -1;

    if (argc == 1)
        ret = chdir("/user/root");
    else
        ret = chdir(argv[1]);
    return ret;
}

struct command __buildin_command[32] = {
    [0] = { .name = "help",     .func = sh_help,   .description = "show shell's all command", },
    [1] = { .name = "reboot",   .func = sh_reboot, .description = "reboot the system", },
    [2] = { .name = "pwd",      .func = sh_pwd,    .description = "show current work directory", },
    [3] = { .name = "cd",       .func = sh_cd,     .description = "change the work directory, only support absolute directory", },
    [4 ... NR_SHELL_COMMAND - 1] = { .func = NULL, },
};

static void print_info(void)
{
    static char *alphaz_info =
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

void tty_task(void)
{
    char code;
    int len;

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
            shell_exec();
            print_prefix();
        } else if (code == '\b')
            shell_pop();
        else {
            printf("%c", code);
            shell_append(code);
        }
    }
}
