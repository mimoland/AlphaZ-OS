/**
 * 进程调度相关
 */

#include <alphaz/sched.h>
#include <alphaz/list.h>
#include <alphaz/malloc.h>
#include <alphaz/string.h>
#include <alphaz/kernel.h>
#include <alphaz/tty.h>

#include <asm/system.h>
#include <asm/sched.h>
#include <asm/cpu.h>
#include <asm/bug.h>


/**
 * 任务链表的头结点
 */
struct list_head task_head;


/**
 * idle进程的task_struct地址
 */
struct task_struct *idle;


/**
 * 时钟中断计数器
 */
unsigned long volatile __ticks_data ticks = INIT_TICKS;

pid_t volatile __pid_data pid = INIT_PID;

/**
 *
 */
static inline void ticks_plus(void)
{
    ticks++;
}


/**
 * 获取当前进程的cpu上下文
 */
struct pt_regs * get_pt_regs(struct task_struct *task)
{
    struct pt_regs *regs;
    regs = (struct pt_regs *)kernel_stack_top(task);
    regs = regs - 1;
    return regs;
}


static struct task_struct *
context_switch(struct task_struct *prev, struct task_struct *next)
{
    switch_to(prev, next, prev);
    return prev;
}


/**
 * schedule是进程的调度器，该方法在就绪进程队列中选出一个进程进行切换
 * 当前进程的调度并不涉及优先级和运行时间的一系列复杂因素，仅仅是将时间片消耗完的进程
 * 移到队尾，然后选下一个进程作为可运行的进程
 */
void schedule(void)
{
    struct task_struct *prev, *next;

    prev = list_first_entry(&task_head, struct task_struct, task);
    list_del(&prev->task);
    list_add_tail(&prev->task, &task_head);
    next = list_first_entry(&task_head, struct task_struct, task);
    next->counter = 1;
    context_switch(prev, next);
}


/**
 * 时钟中断处理函数
 */
void do_timer(void)
{
    ticks_plus();
    current->counter--;
    if(current->counter > 0) {
        return;
    }
    schedule();
}


/**
 * 创建idle进程
 */
static void setup_idle_process(void)
{
    /* 其中包括内核栈 */
    struct task_struct *ts = (struct task_struct *)alloc_page(0, 1);
    memset(ts, 0, sizeof(struct task_struct));

    ts->state = TASK_RUNNING;
    ts->flags = 0;

    ts->stack = alloc_page(0, 1);
    ts->pid = 0;
    ts->prio = LOWEST_PRIO;
    ts->counter = 1;

    strcpy(ts->comm, "idle");

    ts->parent = NULL;
    list_head_init(&ts->children);

    ts->thread.esp0 = (u32)kernel_stack_top(ts);
    ts->thread.esp = (u32)user_stack_top(ts);
    ts->mm = NULL;
    ts->signal = 0;

    idle = ts;

    tss.esp0 = ts->thread.esp0;

    list_add(&ts->task, &task_head);
}


/**
 * 任务(进程)初始化，创建第一个进程
 * 该函数应该在内核全部初始化完成后调用，即放在kernel_main()最后调用，该函数会创建一个init
 * 进程，用于以后的初始化
 */
void task_init(void)
{
    list_head_init(&task_head);
    setup_counter();
    setup_idle_process();
}
