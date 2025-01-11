#include "ionumber.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/logger.h"

static const char *DIGITS = "0123456789";

struct ast_ionumber *ast_parse_ionumber(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (!token || token->type != TOKEN_WORD)
    {
        return NULL;
    }

    char *val = token->value.c;
    for (size_t i = 0; val[i]; i++)
    {
        if (!strchr(DIGITS, val[i]))
        {
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
    return number;
}

int ast_eval_ionumber(struct ast_ionumber *node, void **out)
{
    if (!out || !*out)
    {
        errx(EXIT_FAILURE, "out is not a valid pointer");
    }

    long long *ptr = *out;
    *ptr = node->value;
    return AST_EVAL_SUCCESS;
}

void ast_free_ionumber(struct ast_ionumber *node)
{
    free(node);
}

void ast_print_ionumber(struct ast_ionumber *node)
{
    logger("%lld", node->value);
}
