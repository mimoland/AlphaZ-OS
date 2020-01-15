/**
 * 进程调度相关
 */

#include <alphaz/sched.h>
#include <alphaz/list.h>
#include <alphaz/malloc.h>
#include <alphaz/string.h>
#include <alphaz/kernel.h>
#include <alphaz/tty.h>

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

/**
 *
 */
static inline void ticks_plus(void)
{
    ticks++;
}


/**
 * 获取当前进程的进程控制块指针
 */
struct task_struct * current(void)
{
    return __current();
}


/**
 * 获取当前进程的cpu上下文
 */
struct thread_struct * get_thread_info(struct task_struct *task)
{
    struct thread_struct *thread;
    thread = (struct thread_struct *)kernel_stack_top(task);
    thread = thread - 1;
    return thread;
}


void copy_thread(struct thread_struct *dest,
                                struct thread_struct *src)
{
    memcpy(dest, src, sizeof(struct thread_struct));
}

u32 schedule(void)
{
    struct thread_struct *thread;
    struct task_struct *curr;

    curr = list_first_entry(&task_head, struct task_struct, task);

    thread = get_thread_info(curr);

    copy_thread(&curr->thread, thread);
    list_del(&curr->task);
    list_add_tail(&curr->task, &task_head);

    /* 切换到下一个进程 */
    curr = list_first_entry(&task_head, struct task_struct, task);
    tss.esp0 = (u32)kernel_stack_top(curr);

    return (u32)(&curr->thread);
}


/**
 * 时钟中断处理函数
 */
void do_timer(void)
{
    // struct task_struct *curr;

    ticks_plus();
    // curr = current();
    // if (curr->count > 0) {
    //     curr->count--;
    //     return;
    // }
    // schedule();
}




/**
 * 初始化任务链表的头结点
 */
static void init_task_head(void)
{
    list_head_init(&task_head);
}


/**
 * 创建init进程
 */
static void setup_init_process(void)
{
    /* 其中包括内核栈 */
    struct task_struct *ts = (struct task_struct *)alloc_page(0, 1);

    ts->pid = 1;
    ts->stack = alloc_page(0, 1);
    ts->count = 1;
    strcpy(ts->comm, "TestA");
    ts->thread.esp = (u32)user_stack_top(ts);

    idle = ts;

    list_add(&ts->task, &task_head);
}


/**
 * 任务(进程)初始化，创建第一个进程
 * 该函数应该在内核全部初始化完成后调用，即放在kernel_main()最后调用，该函数会创建一个init
 * 进程，用于以后的初始化
 */
void task_init(void)
{
    // init_ticks();
    setup_counter();
    init_task_head();
    /**/
    setup_init_process();
}
