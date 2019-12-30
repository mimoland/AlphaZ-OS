#ifndef _ALPHAZ_MM_H_
#define _ALPHAZ_MM_H_

#include <alphaz/type.h>

/* 内存信息所在的地址，跟loader中的一致 */
#define MEM_INFO_ADDR   0x600

/* mem_map的起始地址 */
#define MEM_MAP_ADDR    0x20010

typedef struct s_page
{
    unsigned int flags;
    unsigned short  count;      /* 该页的引用数 */

} Page;

void mm_init();

#endif
