#include <alphaz/type.h>
#include <alphaz/sched.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/cpu.h>
#include <asm/sched.h>


/**
 * setup_counter - 设置8253计数器
 *
 * pc上计数器的输入频率为1193180HZ
 */
void setup_counter(void)
{
    unsigned long t = 1193180;
    outb(0x43, 0x36); /* 使用二进制 模式3 先读写低字节再读写高字节 计数器0 */
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
