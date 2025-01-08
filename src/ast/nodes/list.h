#ifndef LIST_H
#define LIST_H

#include <stddef.h>

#include "lexer/lexer.h"
#include "node.h"
#include "utils/linked_list.h"

struct ast_list
{
    struct linked_list *list;
};

struct ast_list *ast_parse_list(struct lexer *lexer);
int ast_eval_list(struct ast_list *node, void **out);
void ast_free_list(struct ast_list *node);

#endif // LIST_H
