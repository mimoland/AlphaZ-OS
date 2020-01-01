#ifndef _ASM_GLOBAL_H_
#define _ASM_GLOBAL_H_

#include <alphaz/type.h>
#include <asm/const.h>
#include <asm/protect.h>
#include <asm/process.h>

extern u8 gdt_ptr[];
extern Descriptor gdt[];

extern u8 idt_ptr[];
extern Gate idt[];

extern Process proc_table[];
extern u8 task_stack[];

extern Tss tss;
extern Process* p_proc_ready;

extern Task task_table[];

#endif
