#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "element.h"
#include "ionumber.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "node.h"

struct ast_redir
{
    struct ast_node *number;
    enum token_type pipe;
    char *file;
};

struct redirection
{
    enum token_type type;
    int (*redir)(struct ast_redir *, void **, struct ast_eval_ctx *);
    char *token;
};

struct ast_redir *ast_parse_redir(struct lexer *lexer);
int ast_eval_redir(struct ast_redir *node, void **out,
                   __attribute((unused)) struct ast_eval_ctx *ctx);
void ast_free_redir(struct ast_redir *node);
void ast_print_redir(struct ast_redir *node);

#endif // !REDIRECTION_H
