#ifndef  _ALPHAZ_STRING_H_
#define _ALPHAZ_STRING_H_

#include <asm/string.h>

#ifndef __HAVE_ARCH_MEMCPY
extern void * memcpy(void *, void *, size_t);
#endif

#ifndef __HAVE_ARCH_MEMSET
extern void * memset(void *, u8, size_t);
#endif

#endif
