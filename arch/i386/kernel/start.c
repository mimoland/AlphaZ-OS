#include <asm/bug.h>
#include <asm/const.h>
#include <asm/start.h>
#include <asm/string.h>
#include <asm/global.h>

void arch_start()
{
    disp_str("Hello World");
    /* 切换gdt */
    memcpy(&gdt,(void *)(*((u32*)(&gdt_ptr[2]))),
                                *((u16*)(&gdt_ptr[0]) + 1));
    u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
    u32* p_gdt_base = (u32 *)(&gdt_ptr[2]);
    *p_gdt_limit = GDT_SIZE * sizeof(Descriptor) - 1;
    *p_gdt_base = (u32)&gdt;

    /* 初始化idt */
    u16* p_idt_limit = (u16*)(&idt_ptr[0]);
    u32* p_idt_base = (u32*)(&idt_ptr[2]);
    *p_idt_limit = IDT_SIZE * sizeof(Gate) - 1;
    *p_idt_base = (u32)&idt;
}
