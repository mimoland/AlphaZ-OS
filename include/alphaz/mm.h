#ifndef _ALPHAZ_MM_H_
#define _ALPHAZ_MM_H_

#include <alphaz/type.h>
#include <alphaz/spinlock.h>
#include <alphaz/list.h>
#include <asm/atomic.h>

#define __packed __attribute__((packed))

struct minfo {
    unsigned int base_addr_low;
    unsigned int base_addr_high;
    unsigned int length_low;
    unsigned int length_high;
    unsigned int type;
} __packed;

struct mm_struct {
    unsigned long *pgd;
};

/* 内存信息所在的地址，跟loader中的一致 */
#define MEM_INFO_ADDR       0xc0000600

/* 标示minfo数组结束的魔数 */
#define MEM_INFO_END_MAGIC  0x3f3f3f3f

/* 页数组的起始地址 */
#define PAGE_ARRAY_ADDR     0xc0300100

/* 页属性 */
#define PAGE_RESERVE        (1UL << 0)   /* 保留页，操作系统不能使用 */
#define PAGE_DMA            (1UL << 1)
#define PAGE_NORMAL         (1UL << 2)
#define PAGE_HIGHMEM        (1UL << 3)

struct page {
    unsigned long flags;
    atomic_t _count;        /* 使用计数 */
    struct list_head list;  /* 页块列表 */
    void *virtual;          /* 内核虚拟地址，为NULL为高端内存 */
};

void mm_init();

#define PRE_PAGE_SIZE   0x1000

/* Page中flags的一些属性 */
#define PAGE_END    ((unsigned int)1 << 31)         /* Page数组是/否结束 */
#define PAGE_KEEP   1                               /* 是否是保留地址   */
#define PAGE_NEXT   2                               /* 是否和下一个页是一个整体 */

/* 不同功能的内存范围 */
/* 保留内存范围，动态分配和用户进程不得使用 */
#define MEM_KEEP_FROM    0
#define MEM_KEEP_END     0x500000

/* 用户可使用的内存 */
#define MEM_ALLOAD       0x500000

#endif
