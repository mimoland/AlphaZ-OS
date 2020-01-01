#include <asm/i8259.h>
#include <asm/int.h>
#include <asm/bug.h>
#include <asm/protect.h>
#include <asm/global.h>


static void init_idt_desc(u8 vector, u8 desc_type,
                                init_handler handler, u8 privilege)
{
    Gate *p_gate = &idt[vector];
    u32 base = (u32)handler;
    p_gate->offset_low = base & 0xffff;
    p_gate->selector = SELECTOR_KERNEL_CS;
    p_gate->dcount = 0;
    p_gate->attr = desc_type | (privilege << 5);
    p_gate->offset_high = (base >> 16) & 0xffff;
}

static inline void init_descriptor(Descriptor* p_desc, u32 base,  \
                                        u32 limit, u16 attribute)
{
    p_desc->limit_low = limit & 0x0FFFF;
    p_desc->base_low = base & 0x0FFFF;
    p_desc->base_mid = (base >> 16) & 0x0FF;
    p_desc->attr1 = attribute & 0xFF;
    p_desc->limit_high_attr2 = ((limit >> 16) & 0x0F) | \
                                ((attribute >> 8) & 0xF0);
    p_desc->base_high = (base >> 24) & 0x0FF;
}


inline u32 seg2phys(u16 seg)
{
    Descriptor* p_dest = &gdt[seg >> 3];
    return (p_dest->base_high << 24 | \
            p_dest->base_mid << 16 | p_dest->base_low);
}

void init_prot()
{
    init_8259A();
    /* 初始化中断向量 */
    // 全部初始化成中断门(没有陷阱门)
    init_idt_desc(INT_VECTOR_DIVIDE,        DA_386IGate,
                    divide_error,           PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_DEBUG,         DA_386IGate,
                    single_step_exception,  PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_NMI,           DA_386IGate,
                    nmi,                    PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_BREAKPOINT,    DA_386IGate,
                    breakpoint_exception,   PRIVILEGE_USER);
    init_idt_desc(INT_VECTOR_OVERFLOW,      DA_386IGate,
                    overflow,               PRIVILEGE_USER);
    init_idt_desc(INT_VECTOR_BOUNDS,        DA_386IGate,
                    bounds_check,           PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_INVAL_OP,      DA_386IGate,
                    inval_opcode,           PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_COPROC_NOT,    DA_386IGate,
                    copr_not_available,     PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_DOUBLE_FAULT,  DA_386IGate,
                    double_fault,           PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_COPROC_SEG,    DA_386IGate,
                    copr_seg_overrun,       PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_INVAL_TSS,     DA_386IGate,
                    inval_tss,              PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_SEG_NOT,       DA_386IGate,
                    segment_not_present,    PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_STACK_FAULT,   DA_386IGate,
                    stack_exception,        PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_PROTECTION,    DA_386IGate,
                    general_protection,     PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_PAGE_FAULT,    DA_386IGate,
                    page_fault,             PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_COPROC_ERR,    DA_386IGate,
                    copr_error,             PRIVILEGE_KRNL);
    /* 8259a相关中断 */
    init_idt_desc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, PRIVILEGE_KRNL);
    init_idt_desc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, PRIVILEGE_KRNL);

    tss.ss0 = SELECTOR_KERNEL_DS;
    init_descriptor(&gdt[INDEX_TSS], \
                    vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss), \
                    sizeof(tss) - 1, DA_386TSS);
    tss.iobase = sizeof(tss);

    /* 填充 GDT 中进程的 LDT 的描述符 */
    int i;
    u16 selector_ldt = INDEX_LDT_FIRST << 3;
    for(i = 0; i < NR_TASKS; i++) {
        init_descriptor(&gdt[selector_ldt >> 3],
            vir2phys(seg2phys(SELECTOR_KERNEL_DS), proc_table[i].ldts),
            LDT_SIZE * sizeof(Descriptor) - 1,
            DA_LDT);
        selector_ldt += 1 << 3;
    }
}


void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
{
    char *err_msg[] = {
        "#DE Divide Error",
        "#DB RESERVED",
        "--  NMI Interrupt",
        "#BP Breakpoint",
        "#OF Overflow",
        "#BR BOUND Range Exceeded",
        "#UD Invalid Opcode (Undefined Opcode)",
        "#NM Device Not Available (No Math Coprocessor)",
        "#DF Double Fault",
        "    Coprocessor Segment Overrun (reserved)",
        "#TS Invalid TSS",
        "#NP Segment Not Present",
        "#SS Stack-Segment Fault",
        "#GP General Protection",
        "#PF Page Fault",
        "--  (Intel reserved. Do not use.)",
        /* 某些不可预见的原因，致使下面的定义会干扰err_msg[vec_no]的访问 */
        /* 解决方式是开启gcc -O2优化 */
        "#MF x87 FPU Floating-Point Error (Math Fault)",
        "#AC Alignment Check",
        "#MC Machine Check",
        "#XF SIMD Floating-Point Exception"
    };

    disp_str("Exception! --> ");
    disp_int(vec_no);
    disp_str(" ");
    disp_str(err_msg[vec_no]);
    disp_str("\n");
    disp_str("EFLAGS:");
    disp_int(eflags);
    disp_str(" CS:");
    disp_int(cs);
    disp_str(" EIP:");
    disp_int(eip);

    if (err_code != 0xffffffff) {
        disp_str(" Error code:");
        disp_int(err_code);
    }
}
