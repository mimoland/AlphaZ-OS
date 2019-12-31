/* 初始化8259 */
#include <asm/io.h>
#include <asm/bug.h>
#include <asm/i8259.h>

void init_8259A()
{
    /* 主片 icw1 */
    outb(INT_M_CTL, 0x11);
    /* 从片 icw1 */
    outb(INT_S_CTL, 0x11);
    /* 主片 icw2 设置中断入口地址为0x20 */
    outb(INT_M_CTLMASK, INT_VECTOR_IRQ0);
    /* 从片 icw2 设置中断入口地址为0x28 */
    outb(INT_S_CTLMASK, INT_VECTOR_IRQ8);
    /* 主片 icw3 ir2对应从片 */
    outb(INT_M_CTLMASK, 0x4);
    /* 从片 icw3 对应主片ir2 */
    outb(INT_S_CTLMASK, 0x2);
    /* 主片 icw4 */
    outb(INT_M_CTLMASK, 0x1);
    /* 从片 icw4 */
    outb(INT_S_CTLMASK, 0x1);
    /* 主片 ocw1 */
    outb(INT_M_CTLMASK, 0xfe);
    /* 从片 ocw1 */
    outb(INT_S_CTLMASK, 0xff);
}

/* 外部中断的统一处理函数 */
void spurious_irq(int irq)
{
    disp_str("spurious_irq: ");
    disp_int(irq);
    disp_str("\n");
}
