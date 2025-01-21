#include "my_strcat.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "xalloc.h"

char *my_strcat(char *dst, const char *src)
{
    size_t len1 = strlen(dst);
    size_t total_len = len1 + strlen(src) + 1;
    dst = xrealloc(dst, total_len);
    size_t i = len1;
    size_t j = 0;
    for (; i < total_len; i++)
    {
        dst[i] = src[j];
        j++;
    }
    return dst;
}

char *my_reverse_strcat(char *src, char *dest)
{
    size_t total_len = strlen(src) + strlen(dest);
    char *result = xmalloc(total_len + 1);
    size_t i = 0;
    size_t j = 0;
    while (dest[j] != 0)
    {
        result[i] = dest[j];
        j++;
        i++;
    }
    j = 0;
    while (src[j] != 0)
    {
        result[i] = src[j];
        j++;
        i++;
    }
    result[i] = 0;
    return result;
}
