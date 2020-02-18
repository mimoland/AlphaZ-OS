#include <alphaz/fs.h>
#include <alphaz/type.h>
#include <alphaz/keyboard.h>
#include <alphaz/tty.h>
#include <alphaz/malloc.h>
#include <alphaz/bugs.h>
#include <alphaz/spinlock.h>
#include <asm/atomic.h>

struct file *stdin;
struct file *stdout;
struct file *stderr;

static ssize_t stdin_read(struct file *filp, char *buf, size_t n, loff_t pos)
{
    return keyboard_read(buf, n);
}

static struct file_operations stdin_operations = {
    .lseek = NULL,
    .read = stdin_read,
    .write = NULL,
    .open = NULL,
    .release = NULL,
};

static loff_t stdout_lseek(struct file *filp, loff_t off, int whence)
{
    return -1;
}

static ssize_t stdout_write(struct file *filp, const char *buf, size_t n, loff_t pos)
{
    return tty_write(buf, n, 0x0f);
}

static struct file_operations stdout_operations = {
    .lseek = stdout_lseek,
    .read = NULL,
    .write = stdout_write,
    .open = NULL,
    .release = NULL,
};

static ssize_t stderr_write(struct file *filp, const char *buf, size_t n, loff_t pos)
{
    return tty_write(buf, n, 0x0c);
}

static struct file_operations stderr_operations = {
    .lseek = NULL,
    .read = NULL,
    .write = stderr_write,
    .open = NULL,
    .release = NULL,
};

static struct file * make_dev_file(struct file_operations *fo)
{
    struct file *filp;

    filp = (struct file *)kmalloc(sizeof(struct file), 0);
    assert(filp != NULL);
    filp->f_dentry = NULL;
    filp->f_op = fo;
    spin_init(&filp->f_lock);
    atomic_set(1, &filp->f_count);
    filp->f_flags = 0;
    filp->f_mode = 0;
    filp->f_pos = 0;
    return filp;
}

void stdio_init(void)
{
    stdin = make_dev_file(&stdin_operations);
    stdout = make_dev_file(&stdout_operations);
    stderr = make_dev_file(&stderr_operations);
}
