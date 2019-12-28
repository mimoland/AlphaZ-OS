#ifndef _ASM_GLOBAL_H_
#define _ASM_GLOBAL_H_

#include <alphaz/type.h>
#include <asm/const.h>
#include <asm/protect.h>

extern u8 gdt_ptr[];
extern Descriptor gdt[];

extern u8 idt_ptr[];
extern Gate idt[];

#endif
