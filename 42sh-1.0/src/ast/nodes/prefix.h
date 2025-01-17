#ifndef PREFIX_H
#define PREFIX_H

#include "lexer/lexer.h"
#include "node.h"

struct ast_prefix
{
    struct token *data;
    struct ast_node *redir;
};

struct ast_prefix *ast_parse_prefix(struct lexer *lexer);
int ast_eval_prefix(struct ast_prefix *node, void **out,
                    __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_prefix(struct ast_prefix *node);
void ast_print_prefix(struct ast_prefix *node);

#endif // !PREFIX_H
