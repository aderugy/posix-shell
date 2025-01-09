#include "clist.h"

#include <err.h>
#include <stdlib.h>

#include "utils/linked_list.h"

/* compound_list =
 *  {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
 */

struct ast_clist *ast_parse_clist(struct lexer *lexer)
{
    struct ast_clist *node = calloc(1, sizeof(struct ast_clist));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    struct token *token = lexer_peek(lexer);
    while (token->type == TOKEN_NEW_LINE)
    {
        lexer_pop(lexer);
        if (token)
            free(token);
        token = lexer_peek(lexer);
    }

    struct ast_node *and_or = ast_create(lexer, AST_AND_OR);
    node->list = list_init();
    list_append(node->list, and_or);
    token = lexer_peek(lexer);
    while (token->type == TOKEN_NEW_LINE || token->type == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer);
        if (token)
            free(token);
        token = lexer_peek(lexer);
        while (token->type == TOKEN_NEW_LINE)
        {
            lexer_pop(lexer);
            if (token)
                free(token);
            token = lexer_peek(lexer);
        }
        struct ast_node *and_or = ast_create(lexer, AST_AND_OR);
        if (!and_or)
        {
            return node;
        }
        list_append(node->list, and_or);
    }
    return NULL;
}
int ast_eval_clist(__attribute((unused)) struct ast_clist *node,
                   __attribute((unused)) void **out)
{
    errx(EXIT_FAILURE, "not implemented");
}
void ast_free_clist(__attribute((unused)) struct ast_clist *node)
{
    errx(EXIT_FAILURE, "not implemented");
}
void ast_print_clist(__attribute((unused)) struct ast_clist *node)
{
    errx(EXIT_FAILURE, "not implemented");
}
