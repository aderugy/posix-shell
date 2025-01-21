#ifndef RUN_COMMAND_H
#define RUN_COMMAND_H

#include <stddef.h>

#include "ast/nodes/eval_ctx.h"

struct runnable
{
    const char *name;
    int (*command)(int, char **, struct ast_eval_ctx *);
};

struct runnables_list
{
    size_t size;
    size_t capacity;
    struct runnable **list;
};

int add_command(const char *name,
                int (*command)(int, char **, struct ast_eval_ctx *));
int del_command(const char *name);
int run_command(int argc, char **argv, struct ast_eval_ctx *ast_eval_ctx);

struct runnable *get_command(const char *name, size_t *index);
#endif // !RUN_COMMAND_H
