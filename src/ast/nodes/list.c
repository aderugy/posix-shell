#include "list.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "node.h"
#include "utils/linked_list.h"
#include "utils/logger.h"

struct ast_list *ast_parse_list(struct lexer *lexer)
{
    logger("parse LIST\n");
    struct ast_node *and_or = ast_create(lexer, AST_AND_OR);
    if (!and_or)
    {
        logger("Exit LIST\n");
        return NULL;
    }

    struct ast_list *node = calloc(1, sizeof(struct ast_list));
    if (!node)
    {
        errx(2, "out of memory");
    }

    node->list = list_init();
    list_append(node->list, and_or);

    struct token *token;
    while ((token = lexer_peek(lexer)))
    {
        if (!token || (token->type != TOKEN_SEMICOLON))
        {
            break;
        }
        else
        {
            lexer_pop(lexer);
            free(token);
            and_or = ast_create(lexer, AST_AND_OR);
            if (!and_or)
            {
                break;
            }

            list_append(node->list, and_or);
        }
    }

    logger("Exit LIST\n");
    return node;
}

int ast_eval_list(__attribute((unused)) struct ast_list *node,
                  __attribute((unused)) void **out)
{
    int return_val = 0;
    for (size_t i = 0; i < node->list->size; i++)
    {
        return_val = ast_eval(list_get(node->list, i), out);
    }
    return return_val;
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
