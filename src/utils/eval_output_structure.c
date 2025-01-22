#include "eval_output_structure.h"

#include <stdlib.h>

#include "logger.h"
#include "xalloc.h"

struct eval_output *eval_output_init(void)
{
    struct eval_output *eval_output = xcalloc(1, sizeof(struct eval_output));

    return eval_output;
}

void eval_output_free(struct eval_output *eval_output)
{
    free(eval_output); // free the struct
}
