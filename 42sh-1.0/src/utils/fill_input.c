#include "fill_input.h"

#include <stdlib.h>
#include <string.h>

#include "xalloc.h"
void fill_input_with_argv(char *list_of_strings[], int argc, int index,
                          char *input_to_fill)
{
    for (int i = index - 1; i < argc; i++)
    {
        size_t len = strlen(list_of_strings[i]);
        input_to_fill = xrealloc(input_to_fill, len + 1);
        strcat(input_to_fill, list_of_strings[i]);
        strcat(input_to_fill, " ");
    }
}
