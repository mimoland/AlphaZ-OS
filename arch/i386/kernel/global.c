#include <alphaz/type.h>
#include <asm/bug.h>
#include <asm/global.h>
#include <asm/const.h>
#include <asm/protect.h>
#include <asm/process.h>

u8 gdt_ptr[6];
Descriptor gdt[GDT_SIZE];

u8 idt_ptr[6];
Gate idt[IDT_SIZE];

Process proc_table[NR_TASKS];
u8 task_stack[STACK_SIZE_TOTAL];

Tss tss;
Process *p_proc_ready;

Task task_table[NR_TASKS];
