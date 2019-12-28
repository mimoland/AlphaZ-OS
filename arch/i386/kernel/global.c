#include <alphaz/type.h>
#include <asm/global.h>
#include <asm/const.h>
#include <asm/protect.h>

u8 gdt_ptr[6];
Descriptor gdt[GDT_SIZE];

u8 idt_ptr[6];
Gate idt[IDT_SIZE];
