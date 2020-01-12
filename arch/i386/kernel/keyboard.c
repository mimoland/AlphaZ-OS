#include <alphaz/kernel.h>
#include <alphaz/keyboard.h>
#include <asm/io.h>

/**
 * 键盘中断（irq1）入口
 */
void __keyboard_handle(void)
{
    u8 scan_code = inb(0x60);
    keyboard_handle(scan_code);
}
