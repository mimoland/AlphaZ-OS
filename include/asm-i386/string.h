#ifndef _ASM_STRING_H_
#define _ASM_STRING_H_

#include <alphaz/type.h>

void memcpy(void *to, void *from, size_t n)
{
    u32 d0, d1, d2;
    asm volatile(
        "nop\n\t"
        "nop\n\t"
        "rep ; movsb\n\t"
        : "=&c"(d0), "=&D"(d1), "=&S"(d2)
        :"0"(n), "1"((u32)to), "2"((u32)from)
        : "memory");
}

#endif
