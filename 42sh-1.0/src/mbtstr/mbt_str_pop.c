#include <err.h>

#include "str.h"

char mbt_str_pop(struct mbt_str *str)
{
    if (!str->size)
    {
        errx(1, "mbt_str_pop: empty string");
    }

    str->size--;

    char c = str->data[str->size];
    str->data[str->size] = 0;

    return c;
}
