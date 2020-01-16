#ifndef _ALPHAZ_SYSCALL_H_
#define _ALPHAZ_SYSCALL_H_

#include <alphaz/type.h>
#include <asm/syscall.h>

#define STDIN_FILENO        0
#define STDOUT_FILENO       1
#define STDERR_FILENO       2


#ifndef __SYSCALL_ARGS_STRUCT
#define __SYSCALL_ARGS_STRUCT
struct syscall_args_struct
{
    unsigned long arg0;     /* 系统调用号，或第一个返回的参数 */
    unsigned long arg1;     /* 第一个参数，或第二个返回的参数 */
    unsigned long arg2;
    unsigned long arg3;
    unsigned long arg4;
    unsigned long arg5;
};
#endif


void syscall_init(void);

typedef void (*syscall) ();

#define SYS_CALL_SIZE   32

/**
 * 系统调用表，在kernel/syscall.c中定义
 */
extern syscall syscall_table[];


/**
 * 系统调用相关功能的具体实现
 */
void sys_get_ticks(void);
void sys_fork(void);
void sys_write(void);
void sys_read(void);
void sys_getpid(void);
void sys_debug(void);

/**
 * 系统调用接口
 */

extern unsigned int get_ticks(void);
extern pid_t fork(void);
extern ssize_t write(int fd, const void *buf, size_t n);
extern ssize_t read(int fd, const void *buf, size_t n);
extern pid_t getpid(void);
extern void debug(void);
#endif
