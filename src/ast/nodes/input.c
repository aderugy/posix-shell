#include "input.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "node.h"
#include "utils/linked_list.h"
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

    struct token *token = lexer_peek(lexer);
    if (!token)
    {
        ast_free_input(input);
        return NULL;
    }

    if (token->type == TOKEN_NEW_LINE)
    {
        free(lexer_pop(lexer));
        return input;
    }

    if (token->type != TOKEN_EOF)
    {
        ast_free_input(input);
        return NULL;
    }

    free(lexer_pop(lexer));
    return input;
}

int ast_eval_input(struct ast_input *node, void **out)
{
    if (node->list == NULL)
    {
        logger("intput.c : eval : exit succes\n");
        return AST_EVAL_SUCCESS;
    }

    return ast_eval(node->list, out);
}

void ast_free_input(struct ast_input *node)
{
    if (node)
    {
        ast_free(node->list);
        free(node);
    }
}

void ast_print_input(__attribute((unused)) struct ast_input *input)
{
    logger("INPUT\n");
    if (!input || !input->list)
    {
        logger("END_INPUT\n");
        return;
    }

    struct ast_node *children = input->list;
    struct ast_list *list_node = children->value;
    struct linked_list *list = list_node->list;

    struct linked_list_element *head = list->head;
    for (size_t i = 0; i < list->size; i++)
    {
        logger(" ");
        ast_print(head->data);
        logger(" ");
    }

    logger("\nEND_INPUT\n");
}
