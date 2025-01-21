#include "stdlib.h"
#include "str.h"

void mbt_str_dtor(struct mbt_str *str)
{
    str->size = 0;
    str->capacity = 0;
    if (str->data)
        free(str->data);
    str->data = NULL;
}
