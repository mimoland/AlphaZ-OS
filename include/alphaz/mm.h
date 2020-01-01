#ifndef _ALPHAZ_MM_H_
#define _ALPHAZ_MM_H_

#include <alphaz/type.h>

/* 内存信息所在的地址，跟loader中的一致 */
#define MEM_INFO_ADDR   0x600

/* mem_map的起始地址 */
#define MEM_MAP_ADDR    0x200100

typedef struct s_page
{
    unsigned long flags;
    unsigned short  count;      /* 该页的引用数 */
    unsigned int fpn;           /* 页号 */
    void *virtual;              /* 映射的内核虚拟内存地址 */
} Page;

extern Page *mem_map;

void mm_init();

#define PRE_PAGE_SIZE   0x1000

/* Page中flags的一些属性 */
#define PAGE_END    ((unsigned int)1 << 31)         /* Page数组是/否结束 */
#define PAGE_KEEP   1                               /* 是否是保留地址   */
#define PAGE_NEXT   2                               /* 是否和下一个页是一个整体 */

/* 不同功能的内存范围 */
/* 保留内存范围，动态分配和用户进程不得使用 */
#define MEM_KEEP_FROM    0
#define MEM_KEEP_END     0x400000

/* 用户可使用的内存 */
#define MEM_ALLOAD       0x400000

#endif
