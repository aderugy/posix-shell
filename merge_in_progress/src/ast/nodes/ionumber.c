#include "ionumber.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/logger.h"

// static const char *DIGITS = "0123456789";

struct ast_ionumber *
ast_parse_ionumber(__attribute((unused)) struct lexer *lexer)
{
    /*    logger("Parse IONUMBER\n");
        struct token *token = lexer_peek(lexer);
        if (!token || token->type != TOKEN_WORD)
        {
            logger("Exit IONUMBER\n");
            return NULL;
        }

        char *val = token->value.c;
        logger("ionumber.c : %s\n", val);
        for (size_t i = 0; val[i]; i++)
        {
            if (!strchr(DIGITS, val[i]))
            {
                logger("Exit IONUMBER\n");
                return NULL;
            }
        }

        struct ast_ionumber *number = calloc(1, sizeof(struct ast_ionumber));
        if (!number)
        {
            errx(EXIT_FAILURE, "out of memory");
        }
        lexer_pop(lexer);
        free(token);

        number->value = strtoll(val, NULL, 10);
        logger("Exit IONUMBER\n");
        return number;*/
    errx(1, "ionumber not exists anymore");
}

int ast_eval_ionumber(struct ast_ionumber *node,
                      __attribute((unused)) void **out,
                      __attribute((unused)) struct ast_eval_ctx *ctx)
{
    return node->value;
}

void ast_free_ionumber(struct ast_ionumber *node)
{
    free(node);
}

void ast_print_ionumber(struct ast_ionumber *node)
{
    logger("%lld", node->value);
}
