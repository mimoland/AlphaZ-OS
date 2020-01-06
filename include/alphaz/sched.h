#ifndef _ALPHAZ_SCHED_H_
#define _ALPHAZ_SCHED_H_

#include <alphaz/type.h>
#include <alphaz/list.h>
#include <asm/cpu.h>

void task_init(void);

/* 进程链表头 */
extern struct list_head task_head;

#define TASK_COMM_LEN      32      /* 进程名的长度 */
#define KERNEL_STACK_SIZE  4096   /* 内核栈的大小 */
#define USER_STACK_SIZE    4096   /* 用户栈的大小 */

struct task_struct
{
    volatile long state;    /* 进程状态，-1不可运行，0可运行 */
    unsigned long flags;    /* 状态标识 */

    void *stack;            /* 用户栈 */
    pid_t pid;              /* 进程id */
    unsigned short prio;    /* 进程优先级 */
    unsigned long  count;   /* 上下文切换计数 */

    char comm[TASK_COMM_LEN];           /* 进程名称 */

    struct task_struct *parent;         /* 父进程 */
    struct task_struct *children;       /* 子进程 */

    /* cpu的上下文信息 */
    struct thread_struct thread;

    struct list_head task;

    /*
     * TODO: 进程所用内存空间的描述，包括页表，空间大小等
     */
};


/**
 * 内核栈的定义方式，task_struct和其内核栈共用一片内存区域
 * task_struct使用低端内存，内核栈使用高端内存，该联合体只是想表达内核栈的定义方式
 */
union task_union
{
    struct task_struct task;
    u8 stack[KERNEL_STACK_SIZE];    /* 内核栈 */
};

/**
 * 获取当前内核栈的栈底，减8是防止i386下没有内核栈的切换时访问ss和esp寄存器引发缺页异常
 */
#define kernel_stack_top(task) (((u8 *)task) + KERNEL_STACK_SIZE - 8)

/**
 * 获取用户栈的栈底
 */
#define user_stack_top(task) (((u8 *)task->stack) + USER_STACK_SIZE)

#endif
