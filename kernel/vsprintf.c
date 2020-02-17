#include <stdarg.h>
#include <alphaz/type.h>
#include <alphaz/stdio.h>
#include <alphaz/string.h>

#include <asm/div64.h>

/* from Linux */
#define SIGN	2		/* unsigned/signed long */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */



static char * number(char *str, char *end, unsigned long long num, int base,
                        int type)
{
	char sign, tmp[66];
    const char *digits;
	int i = 0;
	static const char small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    static const char large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    digits = (type & LARGE) ? large_digits : small_digits;
	if (base < 2 || base > 36) return str;

    sign = 0;
    if(type & SIGN) {
        if ((signed long)num < 0) {
            sign = '-';
            num = -(signed long)num;
        }
    }

    if (num == 0)
		tmp[i++] = '0';
	else while (num != 0)
		tmp[i++] = digits[do_div(num, base)];

    if (sign) {
        if (str < end)
            *str++ = sign;
    }
    if (type & SPECIAL) {
        if (base == 8) {
            if (str < end)
                *str++ = '0';
        }
        if (base == 16) {
            if (str < end)
                *str++ = '0';
            if (str < end)
                *str++ = digits[33];
        }
    }

	while (str < end && i > 0)
		*str++ = tmp[--i];

	*str = 0;
	return str;
}


/**
 * vsprintf - 格式化字符串，将其放在指定缓冲区中
 * @buf:    格式化后的字符串所放的缓冲区
 * @fmt:    要格式化成的字符串格式
 * @args:   参数列表
 */
int vsprintf(char *buf, const char *fmt, va_list args)
{
    return vsnprintf(buf, INT_MAX, fmt, args);
}


/**
 * vsnprintf - 格式化字符串，将其放在指定缓冲区中
 * @buf:    格式化后的字符串所放的缓冲区
 * @size:   缓冲区的大小
 * @fmt:    要格式化成的字符串格式
 * @args:   参数列表
 */
int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    char c, *str, *end, *s;
    int flags, base;
    unsigned long long tmp;
    end = buf + size;

    for (str = buf; *fmt != 0; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        ++fmt;              /* 跳过百分号 */
        flags = 0;
        base = 10;

        switch (*fmt) {
        case 'c':
            c = (unsigned char)va_arg(args, int);
            if (str < end)
                *str = c;
            ++str;
            continue;

        case 's':
            s = va_arg(args, char *);
            for (; *s != 0; ++s) {
                if (str < end)
                    *str = *s;
                str++;
            }
            continue;

        case 'p':
            flags |= SPECIAL;
            tmp = (unsigned long long)(unsigned long)va_arg(args, void *);
            str = number(str, end, tmp, 16, flags);
            continue;

        case '%':
            if (str < end)
                *str = '%';
            ++str;
            continue;

        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            break;
        }

        /* 这里为了方便，将所有参数都当做int处理，尽管这不正确 */
        tmp = (unsigned long long)va_arg(args, int);
        str = number(str, end, va_arg(args, int), base, flags);
    }
    *str = 0;
    return (str - buf);
}


/**
 * sprintf - 将格式化字符串输出到缓冲区
 * @buf: 格式化字符串输出的缓冲区
 * @fmt: 要格式化成的字符串格式
 */
int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf, INT_MAX, fmt, args);
	va_end(args);
	return i;
}
