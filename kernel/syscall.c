#include <alphaz/syscall.h>
#include <alphaz/sched.h>
#include <alphaz/tty.h>
#include <alphaz/kernel.h>
#include <alphaz/keyboard.h>
#include <asm/syscall.h>
#include <asm/unistd.h>

syscall syscall_table[SYS_CALL_SIZE];


void sys_get_ticks(void)
{
    SYSCALL_RETURN(ticks);
}


void sys_write(void)
{
    int fd;
    void *buf;
    size_t n;
    SYSCALL_ARGS_3(fd, buf, n);

    /* TODO: 这里应该根据进程的打开的文件信息进行判断 */
    if(fd == STDOUT_FILENO) {
        n = tty_write(buf, n, 0x0f);
    }

    SYSCALL_RETURN(n);
}


void sys_read(void)
{
    int fd;
    void *buf;
    size_t n;
    SYSCALL_ARGS_3(fd, buf, n);

    /* TODO: 这里应该根据进程的打开的文件信息进行判断 */
    if(fd == STDIN_FILENO) {
        n = keyboard_read((char *)buf, n);
    }
    SYSCALL_RETURN(n);
}


void sys_getpid(void)
{
    SYSCALL_RETURN(current->pid);
}


void sys_reboot(void)
{
    __sys_reboot();
}

void sys_debug(void)
{
    struct task_struct *p = current;
    struct pt_regs *regs = get_pt_regs(p);
    printk("%x pid: %d esp0: %x esp: %x\n", (u32)p, p->pid, p->thread.esp0, regs->esp);
}


/**
 * 初始化系统调用表
 */
static inline void setup_syscall_table(void)
{
    syscall_table[__NR_getticks] = sys_get_ticks;
    syscall_table[__NR_fork] = sys_fork;
    syscall_table[__NR_read] = sys_read;
    syscall_table[__NR_write] = sys_write;
    syscall_table[__NR_getpid] = sys_getpid;
    syscall_table[__NR_sleep] = sys_sleep;

    syscall_table[__NR_reboot] = sys_reboot;
    syscall_table[__NR_debug] = sys_debug;
}


void syscall_init(void)
{
    setup_syscall_table();
}
