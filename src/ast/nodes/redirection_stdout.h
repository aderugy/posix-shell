#ifndef REDIRECTION_STDOUT_H
#define REDIRECTION_STDOUT_H
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "node.h"
#include "redirection_stdin.h"

int redir_stdout_file(struct ast_redir *redir, __attribute((unused)) void **out,
                      __attribute((unused)) struct ast_eval_ctx *ctx);
int redir_stdout_file_a(struct ast_redir *redir,
                        __attribute((unused)) void **out,
                        __attribute((unused)) struct ast_eval_ctx *ctx);
int redir_stdout_file_notrunc(struct ast_redir *redir,
                              __attribute((unused)) void **out,
                              __attribute((unused)) struct ast_eval_ctx *ctx);

#endif // !REDIRECTION_STDOUT_H
