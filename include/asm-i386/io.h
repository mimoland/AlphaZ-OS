#ifndef _ASM_IO_H_
#define _ASM_IO_H_

#include <alphaz/type.h>

static inline void outb(u16 port, u8 value)
{
    asm volatile(
        "movw %0, %%dx\n\t"
        "movb %1, %%al\n\t"
        "out %%al, %%dx\n\t"
        "nop\n\t"
        "nop\n\t"
        :
        :"r"(port), "r"(value)
        :"edx");
}

static inline u8 inb(u16 port)
{
    u8 val;
    asm volatile(
        "movw %1, %%dx\n\t"
        "in %%dx, %%al\n\t"
        "nop\n\t"
        "nop\n\t"
        :"=a"(val)
        :"r"(port)
        :"edx");
    return val;
}


static inline u32 readl(u32 addr)
{
    u32 d0;
    asm volatile(
        "movl (%%esi), %%eax\n\t"
        :"=&a"(d0)
        :"S"(addr)
    );
    return d0;
}

static inline u8 readb(u32 addr)
{
    u8 d0;
    asm volatile(
        "movb (%%esi), %%al\n\t"
        :"=&a"(d0)
        :"S"(addr)
    );
    return d0;
}

#endif
