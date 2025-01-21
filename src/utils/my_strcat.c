#include "my_strcat.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char *my_strcat(char *dst, const char *src)
{
    size_t len1 = strlen(dst);
    size_t total_len = len1 + strlen(src) + 1;
    dst = realloc(dst, total_len);
    size_t i = len1;
    size_t j = 0;
    for (; i < total_len; i++)
    {
        dst[i] = src[j];
        j++;
    }
    return dst;
}
