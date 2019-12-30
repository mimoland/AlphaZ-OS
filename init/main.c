/*
 * 内核初始化入口
 */

#include <asm/start.h>
#include <asm/bug.h>
#include <alphaz/mm.h>

int kernel_main()
{
    disp_str("Kernel Main\n");
    mm_init();
    hlt();
    return 0;
}
