#include "list.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "node.h"
#include "utils/err_utils.h"
#include "utils/linked_list.h"
#include "utils/logger.h"

struct ast_list *ast_parse_list(struct lexer *lexer)
{
    struct ast_list *node = calloc(1, sizeof(struct ast_list));
    CHECK_MEMORY_ERROR(node);

    logger("PARSE LIST\n");
    struct ast_node *and_or = ast_create(lexer, AST_AND_OR);
    if (!and_or)
    {
        goto error;
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

    logger("Exit LIST (SUCCESS)\n");
    return node;
error:
    ast_free_list(node);
    logger("Exit LIST (ERROR)\n");
    return NULL;
}

int ast_eval_list(struct ast_list *node, __attribute((unused)) void **out,
                  struct ast_eval_ctx *ctx)
{
    int return_val = 0;
    for (size_t i = 0; i < node->list->size; i++)
    {
        return_val = ast_eval(list_get(node->list, i), out, ctx);
    }
    return return_val;
}

void ast_free_list(struct ast_list *node)
{
    if (node->list)
    {
        list_free(node->list, (void (*)(void *))ast_free);
    }
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
