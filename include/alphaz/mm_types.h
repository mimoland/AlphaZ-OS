#ifndef _ALPHAZ_MM_TYPES_H_
#define _ALPHAZ_MM_TYPES_H_

#include <alphaz/list.h>
#include <asm/atomic.h>

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

struct page {
    unsigned long flags;
    atomic_t _count;        /* 使用计数 */
    struct list_head list;  /* 页块列表 */
    void *virtual;          /* 内核虚拟地址，为NULL为高端内存 */
};

#endif
