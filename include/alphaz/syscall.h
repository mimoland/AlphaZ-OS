#ifndef _ALPHAZ_SYSCALL_H_
#define _ALPHAZ_SYSCALL_H_

#include <asm/syscall.h>

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
void sys_syscall_test(void);

/**
 * 系统调用接口
 */

extern void get_ticks(void);
extern void syscall_test(void);


#endif
