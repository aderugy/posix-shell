#ifndef COMMANDS_H
#define COMMANDS_H

#include "ast/nodes/eval_ctx.h"

#define ECHO "echo"
#define TRUE "true"
#define FALSE "false"
#define EXIT "exit"
#define CD "cd"
#define EXPORT "export"
#define UNSET "unset"
#define DOT "."

int register_commands(void);
int unregister_commands(void);

int echo(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int true_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int false_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int exit_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int cd(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int export_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int unset_builtin(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);
int dot(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);

#endif // !COMMANDS_H
