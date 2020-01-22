#ifndef _ASM_SYSCALL_H_
#define _ASM_SYSCALL_H_

#include <alphaz/type.h>
#include <asm/cpu.h>

#define __SYSCALL_ARGS_1(a1)  do {                  \
    struct pt_regs * _regs = get_pt_regs(current);  \
    a1 = (typeof(a1))_regs->ebx;                    \
} while (0)

#define __SYSCALL_ARGS_2(a1, a2) do {               \
    struct pt_regs * _regs = get_pt_regs(current);  \
    a1 = (typeof(a1))_regs->ebx;                    \
    a2 = (typeof(a2))_regs->ecx;                    \
} while (0)

#define __SYSCALL_ARGS_3(a1, a2, a3) do {           \
    struct pt_regs * _regs = get_pt_regs(current);  \
    a1 = (typeof(a1))_regs->ebx;                    \
    a2 = (typeof(a2))_regs->ecx;                    \
    a3 = (typeof(a3))_regs->edx;                    \
} while (0)

#define __SYSCALL_RETURN(r) do {                    \
    struct pt_regs * _regs = get_pt_regs(current);  \
    _regs->eax = (unsigned long)r;                  \
} while (0)

void __sys_reboot(void);

unsigned long __syscall(int no, int n, ...);

/* 系统调用接口 */
unsigned int get_ticks(void);

pid_t fork(void);

ssize_t write(int fd, const void *buf, size_t n);

ssize_t read(int fd, const void *buf, size_t n);

pid_t getpid(void);

void sleep(unsigned long second);

void msleep(unsigned long ms);

void reboot(void);

void debug(void);

#endif
