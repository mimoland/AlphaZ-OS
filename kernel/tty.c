#include <alphaz/type.h>
#include <alphaz/console.h>
#include <alphaz/kernel.h>
#include <alphaz/stdio.h>
#include <alphaz/unistd.h>
#include <alphaz/keyboard.h>
#include <alphaz/spinlock.h>
#include <alphaz/string.h>
#include <alphaz/bugs.h>
#include <alphaz/dirent.h>
#include <alphaz/ctype.h>
#include <alphaz/fcntl.h>

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
        if (__buildin_command[i].func == NULL) continue;
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

    if (ret == -1)
        printf("error: %s is not a valid directory\n", argv[1]);
    return ret;
}

static int sh_ls(int argc, char *argv[])
{
    static char buf[256];
    struct DIR *dir;
    struct dirent *d;
    int len, size, i;
    short year, mon, day, hour, min;

    if (argc == 1) {
        len = getcwd(buf, 255);
        if (len == -1)
            return -1;
        dir = opendir(buf);
    } else
        dir = opendir(argv[1]);

    if (!dir)
        return -1;
    while ((d = readdir(dir))) {
        year = (d->wdata >> 9) + 1980;
        mon = (d->wdata >> 5) & 0xf;
        day = d->wdata & 0x1f;
        hour = (d->wtime >> 11);
        min = (d->wtime >> 5) & 0x3f;
        size = (unsigned)d->size;
        if (d->type == FS_ATTR_DIR) {
            i = 0;
            while (d->name[i])
                i++;
            d->name[i++] = '/';
            d->name[i] = 0;
        }

        printf(" %-11s %4d  %d/%02d/%02d %02d:%02d\n", d->name, size, year, mon,
               day, hour, min);
    }
    closedir(dir);
    return 0;
}

static int sh_cat(int argc, char *argv[])
{
    static char buf[256];
    int fd, len;

    if (argc != 2) {
        printf(" error: no input file\n");
        return -1;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        printf(" error: no such file\n");
        return -1;
    }

    while((len = read(fd, buf, 256)) != EOF)
        write(STDOUT_FILENO, buf, len);
    printf("\n");
    close(fd);
    return 0;
}

struct command __buildin_command[32] = {
    [0] = { .name = "help",     .func = sh_help,   .description = "show all commands", },
    [1] = { .name = "reboot",   .func = sh_reboot, .description = "reboot the system", },
    [2] = { .name = "pwd",      .func = sh_pwd,    .description = "print current work directory", },
    [3] = { .name = "cd",       .func = sh_cd,     .description = "change the work directory, only support absolute directory", },
    [4] = { .name = "ls",       .func = sh_ls,     .description = "show all files in a directory"},
    [5] = { .name = "cat",      .func = sh_cat,    .description = "read file and print on the standard output"},
    [6 ... NR_SHELL_COMMAND - 1] = { .func = NULL, },
};

static void print_info(void)
{

    char buf[128];
    int fd, len;

    fd = open("/etc/welcome", O_RDONLY);

    if (fd == -1)
        return;

    while ((len = read(fd, buf, 127)) != EOF) {
        buf[len] = 0;
        printf("%s", buf);
    }
    close(fd);
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

    shell_buf.tail = 0;
    memset(shell_buf.buf, 0, SHELL_BUF_SIZE);

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
