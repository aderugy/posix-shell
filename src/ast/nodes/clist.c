#include "clist.h"

#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/linked_list.h"
#include "utils/logger.h"

/* compound_list =
 *  {'\n'} and_or { ( ';' | '\n' ) {'\n'} and_or } [';'] {'\n'} ;
 */

struct ast_clist *ast_parse_clist(struct lexer *lexer)
{
    logger("Parse CLIST\n");
    struct ast_clist *node = calloc(1, sizeof(struct ast_clist));
    if (!node)
    {
        errx(2, "out of memory");
    }

    struct token *token;
    while ((token = lexer_peek(lexer))->type == TOKEN_NEW_LINE)
    {
        lexer_pop(lexer);
        free(token);
    }

    struct ast_node *and_or = ast_create(lexer, AST_AND_OR);
    if (!and_or)
    {
        free(node);
        return NULL;
    }

    node->list = list_init();
    list_append(node->list, and_or);

    token = lexer_peek(lexer);
    while (token->type == TOKEN_NEW_LINE || token->type == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer);
        free(token);

        token = lexer_peek(lexer);
        while (token->type == TOKEN_NEW_LINE)
        {
            lexer_pop(lexer);
            free(token);

            token = lexer_peek(lexer);
        }

        struct ast_node *and_or = ast_create(lexer, AST_AND_OR);
        if (!and_or)
        {
            return node;
        }

        list_append(node->list, and_or);

        token = lexer_peek(lexer);
        if (!token)
        {
            errx(2, "clist : not token\n");
        }
    }

    return node;
}

int ast_eval_clist(struct ast_clist *node, __attribute((unused)) void **out,
                   struct ast_eval_ctx *ctx)
{
    if (!node->list->size)
    {
        return 0;
    }

    for (size_t i = 0; i < node->list->size - 1; i++)
    {
        struct ast_node *children = list_get(node->list, i);
        ast_eval(children, NULL, ctx);
    }

    struct ast_node *children = list_get(node->list, node->list->size - 1);
    return ast_eval(children, NULL, ctx);
}

void ast_free_clist(struct ast_clist *node)
{
    list_free(node->list, (void (*)(void *))ast_free);
    free(node);
}

void ast_print_clist(__attribute((unused)) struct ast_clist *node)
{
    if (!node->list)
    {
        logger(";");
        return;
    }

    struct linked_list *list = node->list;
    struct linked_list_element *el = list->head;
    while (el)
    {
        ast_print(el->data);
        logger(" ");

        el = el->next;
    }

    logger(";");
}
