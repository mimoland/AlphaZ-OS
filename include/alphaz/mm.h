#ifndef _ALPHAZ_MM_H_
#define _ALPHAZ_MM_H_

#include <alphaz/type.h>
#include <alphaz/mmzone.h>

#define __packed __attribute__((packed))

extern struct zone mm_zones[MAX_NR_ZONES];

#define KERNEL_BASE         0xc0000000

/* 内存信息所在的地址，跟loader中的一致 */
#define MEM_INFO_ADDR       0xc0000600

/* 标示minfo数组结束的魔数 */
#define MEM_INFO_END_MAGIC  0x3f3f3f3f

/* 页数组的起始地址 */
#define PAGE_ARRAY_ADDR     0xc0300100

/* 页属性 */
#define PF_RESERVE          (1UL << 0)   /* 保留页，操作系统不能使用 */
#define PF_DMA              (1UL << 1)
#define PF_NORMAL           (1UL << 2)
#define PF_HIGHMEM          (1UL << 3)

void mm_init();

#define PAGE_SIZE       0x1000

#endif
