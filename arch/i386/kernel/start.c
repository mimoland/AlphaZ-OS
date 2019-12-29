#include <asm/bug.h>
#include <asm/const.h>
#include <asm/start.h>
#include <asm/string.h>
#include <asm/global.h>
#include <asm/int.h>

void arch_start()
{
    /* 由于一些不可预知的原因，要对bug.c中的全局变量首先初始化，一般的初始化方法无效 */
    bug_init();
    /* 切换gdt */
    asm volatile("sgdt %0"::"m"(gdt_ptr):"memory");
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

    init_port();

    asm volatile("lgdt %0"::"m"(gdt_ptr));
    asm volatile("lidt %0"::"m"(idt_ptr));
    disp_str("Hello Kernel\n");
}
