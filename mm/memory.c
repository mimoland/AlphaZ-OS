#include <alphaz/kernel.h>
#include <alphaz/malloc.h>
#include <alphaz/mm.h>
#include <alphaz/mm_types.h>
#include <alphaz/mmzone.h>
#include <alphaz/string.h>
#include <alphaz/type.h>
#include <alphaz/gfp.h>
#include <alphaz/config.h>
#include <alphaz/page.h>

#include <asm/bug.h>
#include <asm/div64.h>
#include <asm/io.h>

struct zone mm_zones[MAX_NR_ZONES] = {
    [ZONE_DMA] = { .name = "DMA", },
    [ZONE_NORMAL] = { .name = "NORMAL", },
    [ZONE_HIGHMEM] = { .name = "HIGHMEM", },
};

static unsigned long zone_begin_addr[MAX_NR_ZONES] = {
    ZONE_DMA_BEGIN, ZONE_NORMAL_BEGIN, ZONE_HIGHMEM_BEGIN,
};
static unsigned long zone_end_addr[MAX_NR_ZONES] = {
    ZONE_DMA_END, ZONE_NORMAL_END, ZONE_HIGHMEM_END,
};

struct page *mem_map;
const struct minfo *minfo_array = (struct minfo *)MEM_INFO_ADDR;

/* 判断当前minfo是否结束 */
static int is_minfo_end(struct minfo *info)
{
    return info->base_addr_low == MEM_INFO_END_MAGIC &&
             info->base_addr_high == MEM_INFO_END_MAGIC;
}

/* 计算内存总大小，包括内存空洞 */
static unsigned long long calc_total_mem(void)
{
    struct minfo *info = (struct minfo *)minfo_array;
    unsigned long long total = 0;

    while (!is_minfo_end(info)) {
        total +=
            info->length_low | ((unsigned long long)info->length_high << 32);
        info++;
    }
    return total;
}

/* 初始化pages数组, 返回pages数组的尾地址 */
static unsigned long init_pages(unsigned long long mem_size, unsigned long mem_map_addr)
{
    int i;
    unsigned long num, kn;
    void *addr;
    struct page *p;

    mem_map_addr = (mem_map_addr / PAGE_SIZE) * PAGE_SIZE + (mem_map_addr % PAGE_SIZE ? PAGE_SIZE : 0);
    mem_map = (struct page *)mem_map_addr;

    do_div(mem_size, PAGE_SIZE);
    num = (unsigned long)mem_size;

    kn = ZONE_NORMAL_END / PAGE_SIZE;
    addr = (void *)0x00;

    p = (struct page *)mem_map;
    for (i = 0; i < num; i++) {
        p->flags = 0;
        atomic_set(0, &p->_count);
        list_head_init(&p->list);
        if (i < kn)
            p->virtual = addr + __KERNEL_OFFSET;
        else
            p->virtual = addr - __USER_OFFSET;
        addr = addr + PAGE_SIZE;
        p++;
    }
    return (unsigned long)(mem_map + num);
}

/* 设置页的属性 */
static inline void setup_pages_flags(unsigned long bi, unsigned long ei,
                                    unsigned long flags)
{
    int i;
    struct page *p = (struct page *)mem_map;

    for (i = bi; i < ei; ++i) {
        (p + i)->flags |= flags;
    }
}

/* 根据minfo数组设置pages属性 */
static int setup_pages_from_minfo(unsigned long num)
{
    unsigned long long addr, limit;
    unsigned long type, bi, ei;
    struct minfo *info = (struct minfo *)minfo_array;

    while (!is_minfo_end(info)) {
        addr = info->base_addr_low | ((unsigned long long)info->base_addr_high << 32);
        limit = info->length_low | ((unsigned long long)info->length_high << 32);
        type = info->type;
        info++;

        if (type == 1) continue;        /* 类型1为操作系统可用的内存 */

        do_div(addr, PAGE_SIZE);
        bi = addr;                      /* 在pages数组中的起始下标 */
        if (bi >= num) continue;        /* 超过实际内存大小，大于pages数组的长度 */
        ei = bi + limit / PAGE_SIZE + (limit % PAGE_SIZE ? 1 : 0);
        setup_pages_flags(bi, ei, PF_RESERVE);
    }
    return 0;
}

/* 在mem_map中标示当前已经使用的内存, 返回已使用的页数 */
static int setup_pages_reserve(unsigned long tail_addr)
{
    int i;
    unsigned long nr;

    nr = (tail_addr - __KERNEL_OFFSET) / PAGE_SIZE + 1;

    for (i = 0; i < nr; i++)
        mem_map[i].flags |= PF_RESERVE;
    return nr;
}

static int init_zones(void)
{
    int i, j;

    for (i = 0; i < MAX_NR_ZONES; i++) {
        mm_zones[i].flags = 0;
        spin_init(&mm_zones[i].lock);
        for (j = 0; j < MAX_ORDER; j++) {
            list_head_init(&mm_zones[i].free_area[j].free_list);
            mm_zones[i].free_area[j].nr_free = 0;
        }
        list_head_init(&mm_zones[i].activate);
        mm_zones[i].first_page = NULL;
        mm_zones[i].nr_pages = 0;
    }

    return 0;
}

/* 设置mm_zones数组 */
static int setup_zones(unsigned long num)
{
    int i;
    unsigned long bi, ei;   /* 起始下标和结束下标 */

    init_zones();
    for (i = 0; i < MAX_NR_ZONES; i++) {
        bi = zone_begin_addr[i] / PAGE_SIZE;
        ei = bi + (zone_end_addr[i] / PAGE_SIZE);
        if (bi >= num) continue;  // 大于实际的内存大小
        if (ei >= num) ei = num;  // 结束下标大于实际内存
        mm_zones[i].first_page = (struct page *)mem_map + bi;
        mm_zones[i].nr_pages = ei - bi;
    }

    return 0;
}

static void show_mem_info(void)
{
    struct minfo *p = (struct minfo *)minfo_array;
    while(!is_minfo_end(p))
    {
        disp_int(p->base_addr_high);
        disp_str(" ");
        disp_int(p->base_addr_low);
        disp_str(" ");
        disp_int(p->length_high);
        disp_str(" ");
        disp_int(p->length_low);
        disp_str(" ");
        disp_int(p->type);
        disp_str("\n");
        p++;
    }
}

void mm_init()
{
    unsigned long long memsize;
    unsigned long pagenum;
    unsigned long tail_addr;

    show_mem_info();
    memsize = calc_total_mem();
#ifdef __ARCH_I386
    if (memsize >= 0xffffffff)
        memsize = 0xffffffff;
#endif

    tail_addr = reset_page_table(memsize);
    tail_addr = init_pages(memsize, tail_addr);

    do_div(memsize, PAGE_SIZE);
    pagenum = memsize;

    setup_pages_from_minfo(pagenum);
    setup_pages_reserve(tail_addr);
    setup_zones(pagenum);
    buddy_system_init();
}
