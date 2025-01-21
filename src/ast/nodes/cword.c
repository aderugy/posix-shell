#define _POSIX_C_SOURCE 200809L
#include "cword.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "eval_ctx.h"
#include "lexer/token.h"
#include "mbtstr/str.h"
#include "node.h"
#include "utils/linked_list.h"
#include "utils/logger.h"
#include "utils/mergestr.h"
#include "utils/xalloc.h"

static int eval_word(const struct ast_cword *node, struct linked_list *out,
                     struct ast_eval_ctx *ctx)
{
    if (!node->next)
    {
        struct eval_output *eval_output = eval_output_init();

        eval_output->value.str = strdup(node->data);

        list_append(out, eval_output);

        return AST_EVAL_SUCCESS;
    }

    struct linked_list *right = NULL;
    if (ast_eval_cword(node->next, right, ctx) != AST_EVAL_SUCCESS)
    {
        return AST_EVAL_ERROR;
    }

    char *right_str = right->head->data;

    struct eval_output *eval_output = eval_output_init();

    eval_output->value.str = merge_str(node->data, right_str);

    list_append(out, eval_output);
    free(right);

    return AST_EVAL_SUCCESS;
}

static int eval_complex_word(const struct ast_cword *node,
                             struct linked_list *out, struct ast_eval_ctx *ctx)
{
    return eval_word(node, out, ctx);
}

static int eval_subshell(__attribute((unused)) const struct ast_cword *node,
                         __attribute((unused)) struct linked_list *out,
                         __attribute((unused)) struct ast_eval_ctx *ctx)
{
    errx(EXIT_FAILURE, "not implemented");
}

static int eval_arith(__attribute((unused)) const struct ast_cword *node,
                      __attribute((unused)) struct linked_list *out,
                      __attribute((unused)) struct ast_eval_ctx *ctx)
{
    errx(EXIT_FAILURE, "not implemented");
}

static int eval_variable(const struct ast_cword *node, struct linked_list *out,
                         struct ast_eval_ctx *ctx)
{
    char *var = ctx_get_variable(ctx, node->data);

    if (!var)
    {
        var = "";
    }
    out = list_init();
    struct eval_output *eval_output = eval_output_init();

    eval_output->value.str = strdup(var);

    list_append(out, eval_output);
    return AST_EVAL_SUCCESS;
}

static int eval_globbing_star(__attribute((unused))
                              const struct ast_cword *node,
                              __attribute((unused)) struct linked_list *out,
                              __attribute((unused)) struct ast_eval_ctx *ctx)
{
    errx(EXIT_FAILURE, "not implemented");
}

static int eval_globbing_qm(__attribute((unused)) const struct ast_cword *node,
                            __attribute((unused)) struct linked_list *out,
                            __attribute((unused)) struct ast_eval_ctx *ctx)
{
    errx(EXIT_FAILURE, "not implemented");
}

struct token_eval_entry
{
    enum token_type type;
    int (*eval_fn)(const struct ast_cword *node, struct linked_list *out,
                   struct ast_eval_ctx *ctx);
};

static const struct token_eval_entry eval_table[] = {
    { TOKEN_WORD, eval_word },
    { TOKEN_COMPLEX_WORD, eval_complex_word },
    { TOKEN_SUBSHELL, eval_subshell },
    { TOKEN_ARITH, eval_arith },
    { TOKEN_VARIABLE, eval_variable },
    { TOKEN_GLOBBING_STAR, eval_globbing_star },
    { TOKEN_GLOBBING_QM, eval_globbing_qm },
};

struct ast_cword *ast_parse_cword_from_token(struct token *token)
{
    if (!token)
    {
        return NULL;
    }

    switch (token->type)
    {
    case TOKEN_WORD:
    case TOKEN_COMPLEX_WORD:
    case TOKEN_SUBSHELL:
    case TOKEN_ARITH:
    case TOKEN_VARIABLE:
    case TOKEN_GLOBBING_STAR:
    case TOKEN_GLOBBING_QM:
        break;
    default:
        return NULL;
    }

    struct ast_cword *node = xcalloc(1, sizeof(struct ast_cword));
    node->type = token->type;
    node->data = strdup(token->value.c);

    if (token->next)
    {
        node->next = ast_parse_cword_from_token(token->next);
    }

    return node;
}

struct ast_cword *ast_parse_cword(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (!token)
    {
        return NULL;
    }

    struct ast_cword *node = ast_parse_cword_from_token(token);
    if (node)
    {
        lexer_pop(lexer);
        token_free(token);
    }

    logger("Parse CWORD (%s)\n", node ? "SUCCESS" : "ERROR");
    return node;
}

int ast_eval_cword(struct ast_cword *node, struct linked_list *out,
                   struct ast_eval_ctx *ctx)
{
    if (!node)
    {
        return AST_EVAL_ERROR;
    }

    for (size_t i = 0; i < sizeof(eval_table) / sizeof(eval_table[0]); i++)
    {
        if (eval_table[i].type == node->type)
        {
            return eval_table[i].eval_fn(node, out, ctx);
        }
    }

    return AST_EVAL_ERROR;
}

void ast_free_cword(struct ast_cword *node)
{
    if (node->next)
    {
        ast_free_cword(node->next);
    }
    free(node->data);
    free(node);
}

void ast_print_cword(struct ast_cword *node)
{
    logger("%s\n", node->data);
    if (node->next)
    {
        ast_print_cword(node->next);
    }
}
