#include "str.h"

// str2 will be freed
void mbt_str_merge(struct mbt_str *str1, struct mbt_str *str2)
{
    if (str2)
    {
        for (size_t i = 0; i < str2->size; i++)
        {
            mbt_str_pushc(str1, str2->data[i]);
        }
        mbt_str_free(str2);
    }
}
