#ifndef REDIRECTION_STDIN_H
#define REDIRECTION_STDIN_H
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "node.h"
struct ast_redir
{
    int number;
    enum token_type pipe;
    char *file;
};

struct redirection
{
    enum token_type type;
    int (*redir)(struct ast_redir *, void **, struct ast_eval_ctx *);
    char *token;
};
int redir_file_stdin(struct ast_redir *node, __attribute((unused)) void **out,
                     __attribute((unused)) struct ast_eval_ctx *ctx);

#endif // !REDIRECTION_STDIN_H
