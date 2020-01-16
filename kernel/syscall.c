#include <alphaz/syscall.h>
#include <alphaz/sched.h>
#include <alphaz/tty.h>
#include <alphaz/kernel.h>
#include <alphaz/keyboard.h>
#include <asm/unistd.h>

syscall syscall_table[SYS_CALL_SIZE];


void sys_get_ticks(void)
{
    struct syscall_args_struct args;
    struct task_struct * task = current();
    struct pt_regs * regs = get_pt_regs(task);
    get_syscall_args(&args, regs);

    args.arg0 = ticks;

    set_syscall_args(&args, regs);
}


void sys_write(void)
{
    int fd;
    void *buf;
    size_t n;
    struct syscall_args_struct args;
    struct task_struct * task = current();
    struct pt_regs * regs = get_pt_regs(task);

    get_syscall_args(&args, regs);
    fd = (int)args.arg1;
    buf = (void *)args.arg2;
    n = (size_t)args.arg3;

    /* TODO: 这里应该根据进程的打开的文件信息进行判断 */
    if(fd == STDOUT_FILENO) {
        n = tty_write(buf, n, 0x0f);
    }

    args.arg0 = n;
    set_syscall_args(&args, regs);
}


void sys_read(void)
{
    int fd;
    size_t n;
    struct syscall_args_struct args;
    struct task_struct * task = current();
    struct pt_regs * regs = get_pt_regs(task);
    get_syscall_args(&args, regs);

    fd = (int)args.arg1;
    n = (size_t)args.arg3;

    /* TODO: 这里应该根据进程的打开的文件信息进行判断 */
    if(fd == STDIN_FILENO) {
        args.arg0 = keyboard_read((char *)args.arg2, n);
    }

    set_syscall_args(&args, regs);
}


void sys_getpid(void)
{
    struct syscall_args_struct args;
    struct task_struct * task = current();
    struct pt_regs * regs = get_pt_regs(task);
    get_syscall_args(&args, regs);

    args.arg0 = current()->pid;

    set_syscall_args(&args, regs);
}


void sys_debug(void)
{
    struct task_struct *p = current();
    struct pt_regs * regs = get_pt_regs(p);

    printk("pid: %d esp0: %x esp: %x\n", p->pid, p->thread.esp0, regs->esp);
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

    syscall_table[__NR_debug] = sys_debug;
}


void syscall_init(void)
{
    setup_syscall_table();
}
