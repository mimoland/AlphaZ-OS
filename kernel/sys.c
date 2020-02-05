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
    /* TODO: 这里应该根据进程的打开的文件信息进行判断 */
    if(fd == STDOUT_FILENO) {
        nbytes = tty_write(buf, nbytes, 0x0f);
    }
    return nbytes;
}

asmlinkage ssize_t sys_read(int fd, void *buf, size_t nbytes)
{
    /* TODO: 这里应该根据进程的打开的文件信息进行判断 */
    if(fd == STDIN_FILENO) {
        nbytes = keyboard_read((char *)buf, nbytes);
    }
    return nbytes;
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
