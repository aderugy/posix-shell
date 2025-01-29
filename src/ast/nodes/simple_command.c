#include "simple_command.h"

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/ast.h"
#include "builtins/builtins.h"
#include "builtins/commands.h"
#include "builtins/run_command.h"
#include "eval_ctx.h"
#include "expansion/vars.h"
#include "lexer/token.h"
#include "node.h"
#include "simple_command_execute_builtin.h"
#include "simple_command_execute_non_builtin.h"
#include "utils/linked_list.h"
#include "utils/logger.h"
#include "utils/naming.h"
#include "utils/xalloc.h"

struct ast_simple_cmd *ast_parse_simple_cmd(struct lexer *lexer)
{
    struct ast_simple_cmd *cmd = xcalloc(1, sizeof(struct ast_simple_cmd));

    struct token *token = NULL;
    cmd->prefix = ast_create(lexer, AST_PREFIX);
    cmd->prefixes = list_init();
    cmd->cmd = NULL;
    cmd->args = list_init();

    struct ast_node *prefix;
    while ((prefix = ast_create(lexer, AST_PREFIX)))
    {
        list_append(cmd->prefixes, prefix);
    }

    logger("Parse SIMPLE_COMMAND\n");
    if (cmd->prefix)
    {
        // prefix { prefix }
        logger("Exit SIMPLE_COMMAND (SUCCESS)\n");
        return cmd;
    }

    struct token *parenthesis = lexer_peek_two(lexer);
    if (parenthesis && parenthesis->type == TOKEN_LEFT_PARENTHESIS)
    {
        goto error;
    }

    // { prefix } WORD { element }
    token = lexer_peek(lexer);
    if (!token
        || (token->quote_type == SHARD_UNQUOTED && !token->next
            && token->value.c && is_keyword(token->value.c)))
    {
        goto error;
    }

    cmd->cmd = ast_create(lexer, AST_COMPLEX_WORD);
    if (!cmd->cmd)
    {
        goto error;
    }

    struct ast_node *element;
    while ((element = ast_create(lexer, AST_ELEMENT)))
    {
        list_append(cmd->args, element);
    }

    logger("Exit SIMPLE_COMMAND (SUCCESS)\n");
    return cmd;
error:
    ast_free_simple_cmd(cmd);
    logger("Exit SIMPLE_COMMAND (ERROR)\n");
    return NULL;
}

struct ast_node *eval_function_command(struct ast_eval_ctx *ctx, char *name,
                                       struct linked_list *args, int *ret_value)
{
    struct ast_node *local_function = ctx_get_function(ctx, name);
    if (local_function) // checks if the function exists in the hashmap
    {
        struct linked_list *params_ctx = ctx_save_spe_vars(ctx);

        *ret_value = ast_eval(local_function, args, ctx);

        ctx_restore_spe_vars(ctx, params_ctx);
        list_free(params_ctx, free);

        return local_function;
    }
    return NULL;
}
int eval_run_command(struct ast_simple_cmd *cmd, char **argv, size_t elt,
                     struct ast_eval_ctx *ctx)
{
    int ret_value = 0;

    struct ast_node *local_function = ctx_get_function(ctx, argv[0]);
    if (local_function) // checks if the function exists in the hashmap
    {
        struct linked_list *params_ctx = ctx_save_spe_vars(ctx);

        ctx_init_local_dollar(ctx);
        ctx_init_local_hashtag(elt, ctx);
        ctx_init_local_UID(ctx);
        ctx_update_local_qm(ctx, 0);
        ctx_reload_local_args(elt, argv, ctx);

        ret_value = ast_eval(local_function, NULL, ctx);

        ctx_restore_spe_vars(ctx, params_ctx);
        list_free(params_ctx, free);
    }
    else
    {
        struct runnable *cmd_runnable =
            get_command(argv[0], NULL); // get the builtin if exists
        if (cmd_runnable) // check if it is a builtin
        {
            ret_value = simple_command_execute_builtin(cmd, argv, ctx);
        }
        else
        {
            argv = xrealloc(
                argv, (elt + 1) * sizeof(char *)); // NULL TERMINATED ARRAY
            argv[elt] = NULL;
            ret_value = simple_command_execute_non_builtin(cmd, argv, ctx, elt);
        }
    }

    for (size_t i = 0; i < elt; i++)
    {
        free(argv[i]);
    }
    free(argv);
    return ret_value;
}

int ast_eval_simple_cmd(struct ast_simple_cmd *cmd,
                        __attribute((unused)) struct linked_list *out,
                        struct ast_eval_ctx *ctx)
{
    if (!cmd->cmd)
    {
        return ast_eval(cmd->prefix, NULL, ctx);
    }

    struct linked_list *cmd_eval = list_init();
    if (ast_eval(cmd->cmd, cmd_eval, ctx) == AST_EVAL_ERROR
        || cmd_eval->size != 1)
    {
        list_free(cmd_eval, (void (*)(void *))eval_output_free);
        return AST_EVAL_ERROR;
    }

    struct eval_output *name_out = cmd_eval->head->data;
    char *name = name_out->value.str;

    int argc = cmd->args->size + 1;
    char **argv = xcalloc(1, sizeof(char *));

    argv[0] = strdup(name);

    size_t elt = 1;
    struct linked_list *linked_list;
    /* LOOP TO TAKE ARGUMENT OF THE CMD AND ADD THEM IN ARGV */
    for (int i = 1; i < argc; i++)
    {
        ctx->check_redir = false;

        struct ast_node *children = list_get(cmd->args, i - 1);
        linked_list = list_init();

        ast_eval(children, linked_list, ctx);
        struct linked_list_element *head = linked_list->head;
        while (head)
        {
            struct eval_output *output = head->data;
            if (output->type == EVAL_STR)
            {
                argv = xrealloc(argv, (elt + 1) * sizeof(char *));

                argv[elt] = strdup(output->value.str);

                elt++;
            }
            head = head->next;
        }
        list_free(linked_list, (void (*)(void *))eval_output_free);
    }

    list_free(cmd_eval, (void (*)(void *))eval_output_free);
    return eval_run_command(cmd, argv, elt, ctx);
}

void ast_free_simple_cmd(struct ast_simple_cmd *cmd)
{
    if (!cmd)
    {
        return;
    }

    if (cmd->prefix)
    {
        ast_free(cmd->prefix);
    }
    list_free(cmd->prefixes, (void (*)(void *))ast_free);

    if (cmd->cmd)
    {
        ast_free(cmd->cmd);
    }
    list_free(cmd->args, (void (*)(void *))ast_free);

    free(cmd);
}

void ast_print_simple_cmd(struct ast_simple_cmd *cmd)
{
    logger("command ");
    if (cmd->cmd)
    {
        ast_print(cmd->cmd);
        logger(" ");
    }

    struct linked_list_element *head = cmd->args->head;
    while (head)
    {
        struct ast_node *node = head->data;
        logger(" ");
        ast_print(node);
        head = head->next;
    }
}
