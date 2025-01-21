#define _POSIX_C_SOURCE 200809L
#include "mergestr.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "xalloc.h"

char *merge_str(char *s1, char *s2)
{
    if (!s1 || !s2)
    {
        if (s1 == s2)
        {
            errx(EXIT_FAILURE, "deux null tu te fous de ma gueule");
        }

        if (s1)
        {
            return strdup(s1);
        }

        return strdup(s2);
    }

    size_t l1 = strlen(s1);
    size_t l2 = strlen(s2);

    char *out = xcalloc(l1 + l2 + 1, sizeof(char));
    memcpy(out, s1, l1);
    memcpy(out + l1, s2, l2);

    return out;
}
