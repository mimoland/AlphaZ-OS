#include <alphaz/type.h>
#include <alphaz/sched.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/cpu.h>
#include <asm/sched.h>


/**
 * setup_thread - 设置thread_struct的内容
 * @thread:    被设置的thread_struct结构体指针
 * @entry:     程序的入口地址
 * @stack:     用户栈首地址
 * @flags:     标志寄存器内容
 */
void setup_thread(struct thread_struct *thread, u32 entry, u32 stack,
                    unsigned int flags)
{
    thread->cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
    thread->ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
    thread->es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
    thread->fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
    thread->ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
    thread->gs = (SELECTOR_VIDEO & SA_RPL_MASK) | SA_RPL3;
    thread->eip = entry;
    thread->esp = stack;
    thread->eflags = flags;
}


/**
 * setup_counter - 设置8253计数器
 *
 * pc上计数器的输入频率为1193180HZ
 */
void setup_counter(void)
{
    unsigned long t = 1193180;
    outb(0x43, 0x34); /* 使用二进制 模式2 先读低字节再读高字节 计数器0 */
    outb(0x40, (u8)(t / HZ));
    outb(0x40, (u8)((t / HZ) >> 8));
    enable_irq(0x00);
}

/**
 * 获取当前进程控制块的地址
 *
 * 由于进程控制块占一个页，每个页都是4k对其的，所以将%esp低12位变为零便是当前进程的进程控制块
 * 地址
 */
struct task_struct * __current(void)
{
    u32 d0;
    asm volatile("andl %%esp, %0":"=r"(d0):"0"(~4095));
    return (struct task_struct *)(d0);
}
