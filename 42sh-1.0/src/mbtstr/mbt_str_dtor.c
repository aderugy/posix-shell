#include "stdlib.h"
#include "str.h"

void mbt_str_dtor(struct mbt_str *str)
{
    str->size = 0;
    str->capacity = 0;

    free(str->data);
}
