#ifndef _ALPHAZ_SCHED_H_
#define _ALPHAZ_SCHED_H_

#include <alphaz/type.h>
#include <alphaz/list.h>
#include <alphaz/string.h>

#include <asm/sched.h>
#include <asm/cpu.h>

#define __ticks_data __attribute__((section(".data")))
#define __pid_data  __attribute__((section(".data")))

void task_init(void);

void schedule(void);

struct task_struct * current(void);

struct pt_regs * get_pt_regs(struct task_struct *task);

void copy_thread(struct thread_struct *, struct thread_struct *);

/*
 * 时钟中断处理程序，在entry.asm中被时钟中断调用
 */
void do_timer(void);

/* 进程链表头 */
extern struct list_head task_head;

#define HZ  100     /* 时钟中断频率100hz */

/**
 * 计数器的初值，它定义为ticks溢出前300s处
 * 之所以定义这样的初值是为了使因溢出造成的问题尽早暴露
 */
#define INIT_TICKS  ((unsigned long)(unsigned int)(-300*HZ))

/**
 * 时钟中断计数器，也是执行进程调度程序的次数
 */
extern unsigned long volatile __ticks_data ticks;


/* 用户进程pid的起始值 */
#define INIT_PID   ((pid_t)1024)

/**
 * pid分配的起始位置
 */
extern pid_t volatile __pid_data pid;

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


extern struct task_struct *idle;


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
