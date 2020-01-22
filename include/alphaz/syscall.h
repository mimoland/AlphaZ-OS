#ifndef _ALPHAZ_SYSCALL_H_
#define _ALPHAZ_SYSCALL_H_

#include <alphaz/type.h>
#include <asm/syscall.h>

#define STDIN_FILENO        0
#define STDOUT_FILENO       1
#define STDERR_FILENO       2


#define SYSCALL_ARGS_1(a1)              __SYSCALL_ARGS_1(a1)
#define SYSCALL_ARGS_2(a1, a2)          __SYSCALL_ARGS_2(a1, a2)
#define SYSCALL_ARGS_3(a1, a2, a3)      __SYSCALL_ARGS_3(a1, a2, a3)
#define SYSCALL_RETURN(r)               __SYSCALL_RETURN(r)

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
void sys_sleep(void);
void sys_debug(void);

/**
 * 系统调用接口
 */

extern unsigned int get_ticks(void);
extern pid_t fork(void);
extern ssize_t write(int fd, const void *buf, size_t n);
extern ssize_t read(int fd, const void *buf, size_t n);
extern pid_t getpid(void);
extern void sleep(unsigned long second);
extern void msleep(unsigned long ms);
extern void debug(void);
#endif
