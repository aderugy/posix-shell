#include "input.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/logger.h"

struct ast_input *ast_parse_input(struct lexer *lexer)
{
    struct ast_input *input = calloc(1, sizeof(struct ast_input));
    if (!input)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    struct ast_node *list = ast_create(lexer, AST_LIST);
    input->list = list;

    struct token *token;
    while ((token = lexer_peek(lexer)))
    {
        if (token && token->type == TOKEN_NEW_LINE)
        {
            free(lexer_pop(lexer));
        }
        else
        {
            break;
        }
    }

    token = lexer_peek(lexer);
    if (!token || token->type != TOKEN_EOF)
    {
        ast_free_input(input);
        return NULL;
    }

    free(lexer_pop(lexer));
    return input;
}

int ast_eval_input(struct ast_input *node, void **out)
{
    return ast_eval(node->list, out);
}

void ast_free_input(struct ast_input *node)
{
    ast_free(node->list);
    free(node);
}

void ast_print_input(__attribute((unused)) struct ast_input *input)
{
    logger("input ");
    ast_print(input->list);
    logger(" endinput");
}
