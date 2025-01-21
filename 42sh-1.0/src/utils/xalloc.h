#ifndef XALLOC_H
#define XALLOC_H

#include <stddef.h>

void *xmalloc(size_t n);
void *xcalloc(size_t nmemb, size_t size);
void *xrealloc(void *ptr, size_t size);

#endif // !XALLOC_H
