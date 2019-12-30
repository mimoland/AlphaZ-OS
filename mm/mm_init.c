#include <alphaz/mm.h>
#include <asm/io.h>
#include <asm/bug.h>

static unsigned int get_mem_size()
{
    u32 addr = MEM_INFO_ADDR;
    if (0xff != readb(addr)) {  // 验证标示，若无标示，则出错
        return 0;
    }
    addr += 2;
    return readl(addr);
}


void mm_init()
{
    unsigned int mem_size = get_mem_size();
    disp_str("memory size: ");
    disp_int(mem_size);
}
