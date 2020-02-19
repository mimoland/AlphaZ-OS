#include <alphaz/bugs.h>
#include <alphaz/unistd.h>
#include <alphaz/sched.h>
#include <alphaz/tty.h>
#include <alphaz/kernel.h>
#include <alphaz/keyboard.h>
#include <alphaz/linkage.h>
#include <asm/unistd.h>

asmlinkage unsigned long sys_get_ticks(void)
{
    return ticks;
}

asmlinkage ssize_t sys_write(int fd, const void *buf, size_t nbytes)
{
    struct file *filp;
    int ret = -1;

    if (fd < 0 || fd >= TASK_MAX_FILE)
        return -1;
    if (nbytes < 0)
        return -1;
    filp = current->files->files[fd];
    assert(filp != NULL);
    assert(filp->f_op != NULL);
    assert(filp->f_op->write != NULL);
    if (filp && filp->f_op && filp->f_op->write)
        ret = filp->f_op->write(filp, buf, nbytes, filp->f_pos);
    return ret;
}

asmlinkage ssize_t sys_read(int fd, void *buf, size_t nbytes)
{
    struct file *filp;
    int ret = -1;

    if (fd < 0 || fd >= TASK_MAX_FILE)
        return -1;
    if (nbytes < 0)
        return -1;
    filp = current->files->files[fd];
    if (filp && filp->f_op && filp->f_op->read)
        ret = filp->f_op->read(filp, buf, nbytes, filp->f_pos);
    return ret;
}

asmlinkage unsigned long sys_getpid(void)
{
    return current->pid;
}

asmlinkage long sys_reboot(void)
{
    __sys_reboot();
    return 0;
}

asmlinkage long sys_debug(void)
{
    struct task_struct *p = current;
    struct pt_regs *regs = get_pt_regs(p);
    printk("%x pid: %d esp0: %x esp: %x\n", (u32)p, p->pid, p->thread.esp0, regs->esp);
    return 0;
}
