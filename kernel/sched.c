/**
 * 进程调度相关
 */

#include <alphaz/sched.h>
#include <alphaz/list.h>
#include <alphaz/malloc.h>
#include <alphaz/string.h>

#include <asm/sched.h>
#include <asm/cpu.h>
#include <asm/bug.h>

void schedule(void)
{

}

/**
 * 任务链表的头结点
 */
struct list_head task_head;

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
static void setup_init_process()
{
    /* 其中包括内核栈 */
    struct task_struct *ts = (struct task_struct *)alloc_page(0, 1);

    ts->pid = 1;
    ts->stack = alloc_page(0, 1);
    ts->count = 0;
    strcpy(ts->comm, "TestA");

    setup_thread(&ts->thread, (u32)TestA, (u32)user_stack_top(ts), 0x1202);

    /**
     * 尽管上面setup_thread中指明了TestA的入口地址，但是由于一些不可预知的原因，其入口地址
     * 并不正确，必须在这里重新显式的初始化。我不清楚引起这个bug的原因是什么，但我认为这应该
     * 和链接器的链接有关。可能是链接器链接时误认为TestA未使用，并未将TestA加入到生成的可执
     * 行文件中
     */
    ts->thread.eip = (u32)TestA;


    tss.ss0 = (u32)kernel_stack_top(ts);

    /* 将当前进程加入进程表 */
    list_add(&ts->task, &task_head);
    /**
     * __switch_to_first_task中会切换堆栈，至此不在使用kernel.asm中定义的临时栈
     * 当进程进入内核态时，内核使用分配给进程的内核栈，用户态时，进程使用分给进程的用户栈
     */
    __switch_to_first_task((u32)(&ts->thread));
}


/**
 * 任务(进程)初始化，创建第一个进程
 * 该函数应该在内核全部初始化完成后调用，即放在kernel_main()最后调用，该函数会创建一个init
 * 进程，用于以后的初始化
 */
void task_init(void)
{
    init_task_head();
    /**/
    setup_init_process();
}
