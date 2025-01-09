#include "list.h"

#include <err.h>
#include <stdlib.h>

#include "node.h"
#include "utils/linked_list.h"
#include "utils/logger.h"

struct ast_list *ast_parse_list(struct lexer *lexer)
{
    struct ast_node *and_or = ast_create(lexer, AST_AND_OR);
    if (!and_or)
    {
        return NULL;
    }

    struct ast_list *node = calloc(1, sizeof(struct ast_list));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    node->list = list_init();
    list_append(node->list, and_or);

    struct token *tok = lexer_peek(lexer);
    while (tok->type == TOKEN_SEMICOLON)
    {
        tok = lexer_pop(lexer);
        and_or = ast_create(lexer, AST_AND_OR);

        if (and_or == NULL)
        {
            return node;
        }

        list_append(node->list, and_or);
    }

    return node;
}

int ast_eval_list(__attribute((unused)) struct ast_list *node,
                  __attribute((unused)) void **out)
{
    errx(EXIT_FAILURE, "not implemented");
}

void ast_free_list(struct ast_list *node)
{
    list_free(node->list, (void (*)())ast_free);
    free(node);
}

void ast_print_list(struct ast_list *node)
{
    logger("list ");
    struct linked_list_element *el = node->list->head;
    while (el)
    {
        ast_print(el->data);
        el = el->next;
        logger(" ");
    }
    logger("endlist");
}
