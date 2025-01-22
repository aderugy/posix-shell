#include "eval_output_structure.h"

#include <stdlib.h>

#include "logger.h"
#include "xalloc.h"

struct eval_output *eval_output_init(enum eval_output_type type)
{
    struct eval_output *eval_output = xcalloc(1, sizeof(struct eval_output));
    eval_output->type = type;

    return eval_output;
}

void eval_output_free(struct eval_output *eval_output)
{
    if (eval_output->type == EVAL_STR)
    {
        free(eval_output->value.str);
    }

    free(eval_output); // free the struct
}
