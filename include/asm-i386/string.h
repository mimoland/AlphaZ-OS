#ifndef _ASM_STRING_H_
#define _ASM_STRING_H_

#include <alphaz/type.h>

#define __HAVE_ARCH_MEMCPY
static inline void * memcpy(void *to, void *from, size_t n)
{
    u32 d0, d1, d2;
    asm volatile(
        "nop\n\t"
        "nop\n\t"
        "rep ; movsb\n\t"
        : "=&c"(d0), "=&D"(d1), "=&S"(d2)
        :"0"(n), "1"((u32)to), "2"((u32)from)
        : "memory");

    return to;
}

#define __HAVE_ARCH_MEMSET
static inline void * memset(void *from, u8 value, size_t n)
{
    u32 d0, d1;
    u8 d2;
    asm volatile(
        "1: movb %%al, (%%edi)\n\t"
        "dec %%ecx\n\t"
        "inc %%edi\n\t"
        "testl %%ecx, %%ecx\n\t"
        "jnz 1b\n\t"
        "2:"
        :"=&c"(d0), "=&D"(d1), "=&a"(d2)
        :"0"(n), "1"((u32)from), "2"(value)
        : "memory");

    return from;
}

#endif
