/*
 * 内核初始化入口
 */

#include <asm/process.h>
#include <asm/start.h>
#include <asm/bug.h>


int kernel_main()
{
    disp_str("kernel_main\n");
    init_process();
    while(1) {}
    return 0;
}
