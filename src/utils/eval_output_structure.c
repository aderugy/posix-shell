#include "eval_output_structure.h"

#include <stdlib.h>

#include "xalloc.h"

struct eval_output *eval_output_init(void)
{
    struct eval_output *eval_output = xcalloc(1, sizeof(struct eval_output));

    eval_output->value.str = xcalloc(1, sizeof(char *));

    return eval_output;
}

void eval_output_free(struct eval_output *eval_output)
{
    if (eval_output->value.str)
    {
        free(eval_output->value.str); // free the char *
    }

    free(eval_output); // free the struct
}
