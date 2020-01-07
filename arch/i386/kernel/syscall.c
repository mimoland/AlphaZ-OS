#include <alphaz/type.h>
#include <asm/syscall.h>
#include <asm/bug.h>

void get_ticks(void)
{
    u32 d0 = 0;
    asm volatile(
        "int $0x80\n\t"
        :"=&a"(d0)
        :"0"(d0));
}


void syscall_test(void)
{
    u32 d0 = 1;
    asm volatile(
        "int $0x80\n\t"
        :"=&a"(d0)
        :"0"(d0));
}
