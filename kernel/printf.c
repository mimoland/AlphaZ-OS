#include <stdarg.h>
#include <alphaz/stdio.h>
#include <asm/bug.h>

int printf(const char *fmt, ...)
{
    char buf[128];
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    disp_str(buf);
    va_end(args);

    return i;
}
