#ifndef LIST_H
#define LIST_H

#include "lexer/lexer.h"
#include "node.h"

struct list_node
{
    struct ast_node **list; // and_or
    size_t size;
};

struct ast_node *ast_parse_list(struct lexer *lexer);
int ast_eval_list(struct ast_node *node, void **out);
void ast_free_list(struct ast_node *node);

#endif // LIST_H
