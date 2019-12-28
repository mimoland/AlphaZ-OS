#include <asm/bug.h>
#include <alphaz/type.h>

int disp_pos = 0;

inline void bug_init()
{
    disp_pos = 0;
}

static inline void disp_char(char c, u8 color, u32 pos)
{
    u16 res = color;
    res = (res << 8) | c;
    asm volatile(
        "movw %%ax, %%gs:(%%edi)\n\t"
        :
        :"a"(res), "D"(pos));
}

static void itoa(int num, char *buf)
{
    char ch;

    *buf++ = '0';
    *buf++ = 'x';

    for (int i = 28; i >= 0; i -= 4) {
        ch = (num >> i) & 0xf;
        if(ch <= 9) ch = ch + '0';
        else ch = ch - 10 + 'A';
        *buf++ = ch;
    }
    *buf = 0;
}

void disp_str(char *buf)
{
    u32 pos = disp_pos;
    for(char* cp = buf; *cp != 0; cp++) {
        if(*cp == '\n') {
            pos = (pos / 160 + 1) * 160;
            continue;
        }
        disp_char(*cp, 0x0f, pos);
        pos += 2;
    }
    disp_pos = pos;
}

void disp_int(int num)
{
    char buf[16];
    itoa(num, buf);
    disp_str(buf);
}