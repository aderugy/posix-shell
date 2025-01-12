#ifndef AST_PREFIX_H
#define AST_PREFIX_H

#include "lexer/lexer.h"

struct ast_prefix
{
    struct ast_node *redir;
};

struct ast_prefix *ast_parse_prefix(struct lexer *lexer);
int ast_eval_prefix(struct ast_prefix *node, void **out);
void ast_free_prefix(struct ast_prefix *node);
void ast_print_prefix(struct ast_prefix *node);

#endif // !AST_PREFIX_H
