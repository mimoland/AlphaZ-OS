#ifndef _ASM_SYSCALL_H_
#define _ASM_SYSCALL_H_

#include <alphaz/syscall.h>
#include <asm/cpu.h>

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

void get_syscall_args(struct syscall_args_struct *, struct pt_regs *);

void set_syscall_args(struct syscall_args_struct *, struct pt_regs *);


/* 系统调用接口 */
unsigned int get_ticks(void);

ssize_t write(int fd, const void *buf, size_t n);

ssize_t read(int fd, const void *buf, size_t n);

#endif
