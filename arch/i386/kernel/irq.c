#include <asm/irq.h>
#include <asm/bug.h>
#include <asm/cpu.h>
#include <asm/i8259.h>

struct gate_struct idt[IDT_SIZE];

static inline void init_idt_desc(u8 vector, u8 desc_type,
                                irq_handler handler, u8 privilege)
{
    struct gate_struct *p_gate = &idt[vector];
    u32 base = (u32)handler;
    p_gate->offset_low = base & 0xffff;
    p_gate->selector = SELECTOR_FLAT_C;
    p_gate->dcount = 0;
    p_gate->attr = desc_type | (privilege << 5);
    p_gate->offset_high = (base >> 16) & 0xffff;
}

static inline void setup_exception_desc(void)
{
    init_idt_desc(INT_VECTOR_DIVIDE,        DA_386IGate,
                    divide_error,           RING0);
    init_idt_desc(INT_VECTOR_DEBUG,         DA_386IGate,
                    single_step_exception,  RING0);
    init_idt_desc(INT_VECTOR_NMI,           DA_386IGate,
                    nmi,                    RING0);
    init_idt_desc(INT_VECTOR_BREAKPOINT,    DA_386IGate,
                    breakpoint_exception,   RING3);
    init_idt_desc(INT_VECTOR_OVERFLOW,      DA_386IGate,
                    overflow,               RING3);
    init_idt_desc(INT_VECTOR_BOUNDS,        DA_386IGate,
                    bounds_check,           RING0);
    init_idt_desc(INT_VECTOR_INVAL_OP,      DA_386IGate,
                    inval_opcode,           RING0);
    init_idt_desc(INT_VECTOR_COPROC_NOT,    DA_386IGate,
                    copr_not_available,     RING0);
    init_idt_desc(INT_VECTOR_DOUBLE_FAULT,  DA_386IGate,
                    double_fault,           RING0);
    init_idt_desc(INT_VECTOR_COPROC_SEG,    DA_386IGate,
                    copr_seg_overrun,       RING0);
    init_idt_desc(INT_VECTOR_INVAL_TSS,     DA_386IGate,
                    inval_tss,              RING0);
    init_idt_desc(INT_VECTOR_SEG_NOT,       DA_386IGate,
                    segment_not_present,    RING0);
    init_idt_desc(INT_VECTOR_STACK_FAULT,   DA_386IGate,
                    stack_exception,        RING0);
    init_idt_desc(INT_VECTOR_PROTECTION,    DA_386IGate,
                    general_protection,     RING0);
    init_idt_desc(INT_VECTOR_PAGE_FAULT,    DA_386IGate,
                    page_fault,             RING0);
    init_idt_desc(INT_VECTOR_COPROC_ERR,    DA_386IGate,
                    copr_error,             RING0);
}

static inline void setup_hwint_desc(void)
{
    /* 8259a相关中断 */
    init_idt_desc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, RING0);
    init_idt_desc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, RING0);
    init_idt_desc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, RING0);
    init_idt_desc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, RING0);
    init_idt_desc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, RING0);
    init_idt_desc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, RING0);
    init_idt_desc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, RING0);
    init_idt_desc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, RING0);
    init_idt_desc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, RING0);
    init_idt_desc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, RING0);
    init_idt_desc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, RING0);
    init_idt_desc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, RING0);
    init_idt_desc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, RING0);
    init_idt_desc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, RING0);
    init_idt_desc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, RING0);
    init_idt_desc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, RING0);
}

static inline void setup_idtr(void)
{
    static struct idtr_struct idtr;
    idtr.len = IDT_SIZE * sizeof(struct gate_struct) - 1;
    idtr.base = (u32)&idt;
    asm volatile("lidt %0"::"m"(idtr));
}

void irq_init(void)
{
    init_8259A();
    setup_exception_desc();
    setup_hwint_desc();
    setup_idtr();
}


/* 异常的统一处理函数 */
void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
{
    disp_str("Exception! --> ");
    disp_int(vec_no);
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


/* 硬件中断的统一处理函数 */
void spurious_irq(int irq)
{
    disp_str("spurious_irq: ");
    disp_int(irq);
    disp_str("\n");
}
