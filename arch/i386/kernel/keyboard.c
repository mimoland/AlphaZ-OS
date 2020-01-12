#include <alphaz/kernel.h>
#include <alphaz/keyboard.h>
#include <asm/irq.h>
#include <asm/io.h>

/**
 * 键盘中断（irq1）入口
 */
void __keyboard_handle(void)
{
    u8 scan_code = inb(0x60);
    keyboard_handle(scan_code);
}


void setup_keyboard_irq(void)
{
    enable_irq(0x01);   /* 开启1号硬件中断，键盘中断 */
}
