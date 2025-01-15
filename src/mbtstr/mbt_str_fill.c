#include "stdlib.h"
#include "str.h"

void mbt_str_fill(struct mbt_str *str, struct mbt_str *to_fill, char c)
{
    while (str->size > to_fill->size)
    {
        mbt_str_pushc(to_fill, c);
    }
}
