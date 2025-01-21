#include "xalloc.h"

#include <err.h>
#include <stdlib.h>

void *xmalloc(size_t n)
{
    return xcalloc(n, 1);
}

void *xcalloc(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);
    if (!ptr)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    return ptr;
}

void *xrealloc(void *ptr, size_t size)
{
    void *nptr = realloc(ptr, size);
    if (!nptr)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    return nptr;
}
