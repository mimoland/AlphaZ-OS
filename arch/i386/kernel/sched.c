#include <alphaz/type.h>
#include <asm/bug.h>
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