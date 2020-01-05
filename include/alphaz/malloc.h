#ifndef _ALPHAZ_MALLOC_H_
#define _ALPHAZ_MALLOC_H_

#include <alphaz/type.h>

void *alloc_page(unsigned long, size_t);

void * malloc(size_t);

void free(void *);

#endif
