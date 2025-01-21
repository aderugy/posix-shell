#ifndef PREFIX_H
#define PREFIX_H

#include "cword.h"
#include "lexer/lexer.h"
#include "node.h"

enum ast_prefix_type
{
    AST_PREFIX_ASSIGNMENT = 1,
    AST_PREFIX_REDIR
};

struct ast_prefix
{
    enum ast_prefix_type type;
    char *name;
    union
    {
        struct ast_cword *word;
        struct ast_node *redir;
    } child;
};

struct ast_prefix *ast_parse_prefix(struct lexer *lexer);
int ast_eval_prefix(struct ast_prefix *node, void **out,
                    __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_prefix(struct ast_prefix *node);
void ast_print_prefix(struct ast_prefix *node);

#endif // !PREFIX_H
