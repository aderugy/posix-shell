#include "ionumber.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/logger.h"

static const char *DIGITS = "0123456789";

struct ast_ionumber *ast_parse_ionumber(struct lexer *lexer)
{
    logger("\t\t\t\tParse IONUMBER\n");
    struct token *token = lexer_peek(lexer);
    if (!token || token->type != TOKEN_WORD)
    {
        logger("\t\t\t\tExit IONUMBER\n");
        return NULL;
    }

    char *val = token->value.c;
    for (size_t i = 0; val[i]; i++)
    {
        if (!strchr(DIGITS, val[i]))
        {
            logger("\t\t\t\tExit IONUMBER\n");
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
    logger("\t\t\t\tExit IONUMBER\n");
    return number;
}

int ast_eval_ionumber(struct ast_ionumber *node,
                      __attribute((unused)) void **out)
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
