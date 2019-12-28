#ifndef _ASM_GLOBAL_H_
#define _ASM_GLOBAL_H_

#include <alphaz/type.h>
#include <asm/const.h>
#include <asm/protect.h>

u8 gdt_ptr[6];
Descriptor gdt[GDT_SIZE];

#endif
