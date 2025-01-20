#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdbool.h>

#include "ast/nodes/eval_ctx.h"

#define ECHO "echo"
#define TRUE "true"
#define FALSE "false"
#define EXIT "exit"
#define CD "cd"
#define EXPORT "export"
#define UNSET "unset"
#define DOT "."
#define BREAK "break"

struct echo_options
{
    bool not_newline;
    bool interpret_backslash;
    bool not_interpret_backslash_default;
    char **str;
};

int echo(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int true_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int false_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int exit_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int cd(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int export_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int unset_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int dot(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int break_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);

#endif // !BUILTINS_H
