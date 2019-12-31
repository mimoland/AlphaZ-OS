#include <alphaz/string.h>
#include <alphaz/malloc.h>
#include <alphaz/mm.h>
#include <asm/io.h>
#include <asm/bug.h>

Page *mem_map;

static unsigned int get_mem_size(u32 addr)
{
    if (0xff != readb(addr)) {  // 验证标示，若无标示，则出错
        return 0;
    }
    addr += 2;
    return readl(addr);
}

/*
 * 返回当前页的地址，并将addr指向下一页
 */
static void * next_virtual_addr(void **addr)
{
    void * res = *addr;
    *addr = (void *)(res + PRE_PAGE_SIZE);
    return res;
}

/* 设置mem_map */
static void setup_mem_map(unsigned size)
{
    int fpn;
    void *addr = (void *)0x00;      /* 页映射的虚拟地址 */
    int keep_page_from, keep_page_end;
    int alload_page_from, alload_page_end;

    mem_map = (Page *)MEM_MAP_ADDR;
    memset(mem_map, 0x0, sizeof(Page)*size);

    /* 初始化内存属性 */
    keep_page_from = MEM_KEEP_FROM / PRE_PAGE_SIZE;
    keep_page_end = MEM_KEEP_END / PRE_PAGE_SIZE - 1;

    for (fpn = keep_page_from; fpn <= keep_page_end; fpn++) {
        (mem_map + fpn)->flags = PAGE_KEEP;
        (mem_map + fpn)->count = 1;
        (mem_map + fpn)->fpn = fpn;
        (mem_map + fpn)->virtual = next_virtual_addr(&addr);
    }

    alload_page_from = MEM_KEEP_END / PRE_PAGE_SIZE;
    alload_page_end = size - 1;
    for(fpn = alload_page_from; fpn <= alload_page_end; fpn++) {
        (mem_map + fpn)->fpn = fpn;
        (mem_map + fpn)->virtual = next_virtual_addr(&addr);
    }

    /* 设置最后一个Page的结束标志 */
    (mem_map + (size - 1))->flags |= PAGE_END;
}

static inline void test_mm()
{
    unsigned int mem_size = get_mem_size(MEM_INFO_ADDR);
    disp_str("total memory: ");
    disp_int(mem_size);
    disp_str("\n");
    Page *p_page = mem_map;
    disp_int((int)p_page);
    int i = 0;
    while (1) {
        i++;
        if(p_page->flags & PAGE_END) break;
        p_page++;
    }

    disp_str(" Page size: ");
    disp_int(sizeof(Page));
    disp_str(" page total: ");
    disp_int(i);
    disp_str("\n");
    disp_int((int)p_page);
    disp_str("\n");
    void *p = malloc(0x1001);
    disp_int((int)p);
    disp_str("\n");
    void *q = malloc(0x1001);
    disp_int((int)q);
    disp_str("\n");
    free(p);
    free(q);
}

void mm_init()
{
    unsigned int mem_size = get_mem_size(MEM_INFO_ADDR);
    setup_mem_map(mem_size / PRE_PAGE_SIZE);

    test_mm();
}
