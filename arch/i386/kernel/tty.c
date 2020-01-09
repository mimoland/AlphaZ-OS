#include <asm/string.h>
#include <asm/tty.h>
#include <asm/irq.h>
#include <asm/io.h>


/**
 * 判断字符是否可以显示
 */
static inline int able_show(char c)
{
    if ((int)c >= 0x20 && (int)c <= 0x7e)
        return 1;
    return 0;
}


/**
 * 获取屏幕当前的游标
 * CRT Controller Registers     Address Register    0x3d4
 *                              Data    Register    0x3d5
 *
 *      Cursor Start Register   0x0a
 *      Cursor End   Register   0x0b
 */
static inline unsigned short get_cursor(void)
{
    unsigned short cur;
    outb(0x3d4, 0x0e);
    cur = inb(0x3d5);
    outb(0x3d4, 0x0f);
    cur = (cur<<8) | inb(0x3d5);
    return cur;
}


/**
 * 设置屏幕当前光标
 */
static inline void set_cursor(unsigned short cur)
{
    outb(0x3d4,0x0e);
    outb(0x3d5, (cur >> 8) & 0xff);
    outb(0x3d4, 0x0f);
    outb(0x3d5, cur & 0xff);
}

/**
 * 屏幕向上卷页
 */
static inline void curl_screen_line(void)
{
    int b0, b1;
    b0 = vir_to_phys(seg_to_phys(SELECTOR_VIDEO), 0);
    b1 = vir_to_phys(seg_to_phys(SELECTOR_VIDEO), 160);
    memcpy((void *)b0, (void *)b1, 160 * 24);
    b0 = vir_to_phys(seg_to_phys(SELECTOR_VIDEO), 160 * 24);
    memset((void *)b0, 0, 160);
}


static inline unsigned short write_char(char c, u8 type, unsigned short cur)
{
    int val = (int)c | ((int)type << 8);

    if (able_show(c)) {
        asm volatile(
            "movw %%ax, %%gs:(%%edi)\n\t"
            :
            :"a"(val), "D"((u32)(cur * 2)));
        ++cur;
    } else {
        switch (c) {
            case '\n':
                cur = (cur / 80 + 1) * 80;
                break;
            case '\t':
                cur = cur + 4;
                break;
            default:
                break;
        }
    }

    if (cur * 2 >= 2 * 80 * 25) {
        curl_screen_line();
        cur -= 80;
    }

    return cur;
}


/**
 * __tty_write - 从屏幕光标处开始写字符
 * @buf:    要输出的数据缓冲区
 * @n:      缓冲区中数据的长度
 * @type:   字符的属性
 */
ssize_t __tty_write(const char *buf, size_t n, u8 type)
{
    unsigned short cur;
    int i;

    cur = get_cursor();
    for (i = 0; i < n; i++) {
        cur = write_char(buf[i], type, cur);
        set_cursor(cur);
    }
    return n;
}
