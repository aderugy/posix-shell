#include "eval_ctx.h"
#define _POSIX_C_SOURCE 200809L

#include <err.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cword.h"
#include "eval_ctx.h"
#include "lexer/lexer.h"
#include "node.h"
#include "prefix.h"
#include "redirection.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

struct ast_prefix *ast_parse_prefix(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (!token)
    {
        return NULL;
    }

    struct ast_prefix *node = xcalloc(1, sizeof(struct ast_prefix));

    char *eq;
    if (token->type == TOKEN_WORD)
    {
        eq = strchr(token->value.c, '=');
        if (eq)
        {
            *(eq++) = 0;
            node->name = strdup(token->value.c);

            // Copies whats after the = to the beginning of the string
            // So that we dont change the pointer
            memmove(token->value.c, eq, strlen(eq) + 1);

            node->type = AST_PREFIX_ASSIGNMENT;
            node->child.word = ast_parse_cword_from_token(token);
            token_free(lexer_pop(lexer));

            logger("PARSE PREFIX (SUCCESS)\n");
            return node;
        }

        ast_free_prefix(node);
        return NULL;
    }

    struct ast_node *redir = ast_create(lexer, AST_REDIRECTION);
    if (!redir)
    {
        ast_free_prefix(node);
        return NULL;
    }

    node->type = AST_PREFIX_REDIR;
    node->child.redir = redir;
    logger("PARSE PREFIX (SUCCESS)\n");
    return node;
}

int ast_eval_prefix(struct ast_prefix *node, struct linked_list *out,
                    struct ast_eval_ctx *ctx)
{
    if (node->type == AST_PREFIX_ASSIGNMENT)
    {
        struct linked_list *linked_list = list_init();
        if (ast_eval_cword(node->child.word, linked_list, ctx)
            == AST_EVAL_ERROR)
        {
            return AST_EVAL_ERROR;
        }
        struct eval_output *eval_output = linked_list->head->data;
        char *value = eval_output->value.str;

        ctx_set_local_variable(ctx, node->name, value);
        free(value);
        return AST_EVAL_SUCCESS;
    }
    else
    {
        return ast_eval(node->child.redir, out, ctx);
    }
}

void ast_free_prefix(struct ast_prefix *node)
{
    if (node)
    {
        if (node->name)
        {
            free(node->name);
        }

        if (node->type == AST_PREFIX_ASSIGNMENT)
        {
            ast_free_cword(node->child.word);
        }
        else
        {
            ast_free(node->child.redir);
        }

        free(node);
    }
}

void ast_print_prefix(struct ast_prefix *node)
{
    if (node)
    {
        if (node->type == AST_PREFIX_ASSIGNMENT)
        {
            logger("%s=", node->name);
            ast_print_cword(node->child.word);
        }
        else
        {
            ast_print(node->child.redir);
        }
    }
}
