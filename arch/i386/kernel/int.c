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

void init_port()
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
