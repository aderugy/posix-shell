#include <ctype.h>
#include "str.h"
#include "view.h"
#include <stdio.h>

void mbt_cview_fprint(struct mbt_cview view, FILE *stream)
{
    for (size_t i = 0; i < view.size; i++)
    {
        unsigned char c = view.data[i];

        if (isprint(c))
        {
            fprintf(stream, "%c", c);
        }
        else
        {
            fprintf(stream, "U+%04X", c);
        }
    }
}
