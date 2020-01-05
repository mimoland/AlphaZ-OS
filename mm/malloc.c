#include <alphaz/mm.h>
#include <alphaz/type.h>

/**
 * alloc_page - 分配连续的物理页，返回物理地址
 * @flags_mask: 页属性
 * @size: 分配的页的数量
 *
 * 若最后未能找到空的物理内存，返回NULL
 */
void *alloc_page(unsigned long flags_mask, size_t size)
{
    int offset;
    Page *p_page = mem_map;
    while ((p_page->flags & PAGE_END) == 0) {
        /* 这里只是简单的检查一下引用是否为0 */
        if (p_page->count != 0) {
            p_page++;
            continue;
        }
        /* 检查后面连续的n个页是否为空页 */
        for (offset = 0; offset < size; offset++) {
            if(((p_page + offset)->flags & PAGE_END) != 0) return NULL;
            if((p_page + offset)->count != 0) {
                p_page = p_page + offset + 1;   /* 将p_page指向下一个页，然后继续 */
                continue;
            }
        }
        break;
    }

    /* 找到最后没有空闲页 */
    if ((p_page->flags & PAGE_END) != 0) return NULL;

    /* 标记 */
    for (offset = 0; offset < size; offset++) {
        (p_page + offset)->count++;
        (p_page + offset)->flags |= flags_mask;
        if (offset < size - 1) (p_page + size)->flags |= PAGE_NEXT;
    }
    return p_page->virtual;
}


/* 通过内核地址得到页号 */
static inline unsigned short get_fpn(void *ptr)
{
    return (((int)ptr) / PRE_PAGE_SIZE);
}


/* 回收页 */
static void free_page(unsigned short fpn)
{
    Page *p_page = mem_map + fpn;

    while (1) {
        p_page->count--;
        if (p_page->flags & PAGE_NEXT) {
            p_page->flags = p_page->flags & !PAGE_NEXT;
        } else {
            break;
        }
    }
}


/**
 * malloc - 分配物理内存，返回物理地址
 * @size: 要分配的内存空间的大小，字节位单位
 *
 * 该函数当前仅处于试验阶段，分配内存时以页位最小单位，所以内存浪费较大
 */
void * malloc(size_t size)
{
    size_t page_s;                      /* 需要的页数 */
    void *ptr;

    if(!size) return NULL;
    page_s = size / PRE_PAGE_SIZE + ((size % PRE_PAGE_SIZE) != 0);
    ptr = alloc_page(0, page_s);

    return ptr;
}


/**
 * free - 回收内存
 * @ptr: 指向要回收内存首部的指针
 *
 * 该函数当前仅处于试验阶段，不能保证正确回收
 */
void free(void *ptr)
{
    unsigned short fpn = get_fpn(ptr);
    free_page(fpn);
}
