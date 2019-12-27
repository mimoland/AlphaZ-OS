/*
 * 内核初始化入口
 */

#include <asm/start.h>

int kernel_main()
{
    arch_start();
    return 0;
}
