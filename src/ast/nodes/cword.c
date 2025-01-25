#define _POSIX_C_SOURCE 200809L
#include "cword.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "eval_ctx.h"
#include "lexer/lexer.h"
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
        logger("no next on cword\n");
        struct eval_output *eval_output = eval_output_init(EVAL_STR);
        eval_output->value.str = strdup(node->data);
        logger("dupped %s\n", eval_output->value.str);

        list_append(out, eval_output);
        return AST_EVAL_SUCCESS;
    }

    struct linked_list *right = list_init();
    if (ast_eval_cword(node->next, right, ctx) != AST_EVAL_SUCCESS)
    {
        list_free(right, (void (*)(void *))eval_output_free);
        return AST_EVAL_ERROR;
    }

    struct linked_list_element *head = right->head;
    while (head)
    {
        struct eval_output *right_eval_output = head->data;
        char *right_str = right_eval_output->value.str;

        logger("LOGGED ON EVAL WORD \n%s\n", right_str);

        struct eval_output *eval_output = eval_output_init(EVAL_STR);
        eval_output->value.str = merge_str(node->data, right_str);

        list_append(out, eval_output);
        head = head->next;
    }
    list_free(right, (void (*)(void *))eval_output_free);
    return AST_EVAL_SUCCESS;
}

static int hs34(struct stream *stream, struct ast_eval_ctx *ctx)
{
    struct lexer *lexer = lexer_create(stream);
    struct ast_node *node = ast_create(lexer, AST_CLIST);
    int return_value = node ? ast_eval(node, NULL, ctx) : 2;
    if (node)
    {
        ast_free(node);
    }

    if (lexer->error)
    {
        warnx("Syntax error");
        return_value = 2;
    }

    lexer_free(lexer);
    return return_value;
}

static int eval_subshell_silent(const struct ast_cword *node,
                                struct ast_eval_ctx *ctx)
{
    pid_t pid;
    pid = fork();
    if (pid == -1)
    {
        errx(EXIT_FAILURE, "subshell: fork failed");
    }

    if (pid == 0)
    {
        struct stream *stream = stream_from_str(node->data);
        int retval = hs34(stream, ctx);
        exit(retval);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

static int sub_subshell(struct mbt_str *stdout_str,
                        const struct ast_cword *node, struct ast_eval_ctx *ctx,
                        struct linked_list *out)
{
    struct eval_output *str = eval_output_init(EVAL_STR);
    stdout_str->data[stdout_str->size - 1] = 0;
    str->value.str = strdup(stdout_str->data);
    mbt_str_free(stdout_str);

    if (node->next)
    {
        struct linked_list *right = list_init();
        if (ast_eval_cword(node->next, right, ctx) != AST_EVAL_SUCCESS)
        {
            eval_output_free(str);
            list_free(right, (void (*)(void *))eval_output_free);
            return AST_EVAL_ERROR;
        }
        logger("FOUND NEXT ON SUBSHELL\n");

        struct linked_list_element *head = right->head;
        while (head)
        {
            struct eval_output *right_eval_output = head->data;
            char *right_str = right_eval_output->value.str;

            struct eval_output *eval_output = eval_output_init(EVAL_STR);
            eval_output->value.str = merge_str(str->value.str, right_str);

            list_append(out, eval_output);
            head = head->next;
        }
        eval_output_free(str);
        list_free(right, (void (*)(void *))eval_output_free);
    }
    else
    {
        list_append(out, str);
    }

    return AST_EVAL_SUCCESS;
}

static int eval_subshell(const struct ast_cword *node, struct linked_list *out,
                         struct ast_eval_ctx *ctx)
{
    if (node->sh_stdout_silent)
    {
        return eval_subshell_silent(node, ctx);
    }

    pid_t pid;
    int pipefd[2];
    char buffer[64];
    if (pipe(pipefd) == -1)
    {
        errx(EXIT_FAILURE, "subshell: pipe failed");
    }

    pid = fork();
    if (pid == -1)
    {
        errx(EXIT_FAILURE, "subshell: fork failed");
    }

    if (pid == 0)
    {
        close(pipefd[0]);

        if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        {
            warnx("dup2");
            exit(EXIT_FAILURE);
        }

        close(pipefd[1]);

        struct stream *stream = stream_from_str(node->data);
        int retval = hs34(stream, ctx);
        exit(retval);
    }
    else
    {
        close(pipefd[1]);
        struct mbt_str *stdout_str = mbt_str_init(63);

        ssize_t r;
        while ((r = read(pipefd[0], buffer, 64 - 1)) > 0)
        {
            for (ssize_t i = 0; i < r; i++)
            {
                if (buffer[i] == '\n')
                {
                    mbt_str_pushc(stdout_str, ' ');
                }
                else
                {
                    mbt_str_pushc(stdout_str, buffer[i]);
                }
            }
        }

        close(pipefd[0]);

        wait(NULL);
        return sub_subshell(stdout_str, node, ctx, out);
    }
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

    struct eval_output *eval_output = eval_output_init(EVAL_STR);

    if (node->next)
    {
        struct linked_list *right = list_init();
        if (ast_eval_cword(node->next, right, ctx) != AST_EVAL_SUCCESS)
        {
            list_free(right, (void (*)(void *))eval_output_free);
            return AST_EVAL_ERROR;
        }

        struct eval_output *right_eval_output = right->head->data;
        char *right_str = right_eval_output->value.str;

        eval_output->value.str = merge_str(var, right_str);

        list_append(out, eval_output);
        list_free(right, (void (*)(void *))eval_output_free);
    }
    else
    {
        eval_output->value.str = strdup(var);

        list_append(out, eval_output);
    }

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
    { TOKEN_SUBSHELL, eval_subshell },
    { TOKEN_ARITH, eval_arith },
    { TOKEN_VARIABLE, eval_variable },
    { TOKEN_GLOBBING_STAR, eval_globbing_star },
    { TOKEN_GLOBBING_QM, eval_globbing_qm },
};

struct ast_cword *ast_parse_cword_from_token(struct token *token,
                                             struct lexer *lexer)
{
    if (!token)
    {
        return NULL;
    }

    struct ast_cword *node = xcalloc(1, sizeof(struct ast_cword));
    switch (token->type)
    {
    case TOKEN_SUBSHELL:
        node->sh_stdout_silent = token->sh_stdout_silent;
        break;
    case TOKEN_WORD:
    case TOKEN_ARITH:
    case TOKEN_VARIABLE:
    case TOKEN_GLOBBING_STAR:
    case TOKEN_GLOBBING_QM:
        break;
    default:
        ast_free_cword(node);
        return NULL;
    }

    node->type = token->type;
    node->data = strdup(token->value.c);

    if (token->next)
    {
        node->next = ast_parse_cword_from_token(token->next, lexer);
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

    struct ast_cword *node = ast_parse_cword_from_token(token, lexer);
    if (node)
    {
        token_free(lexer_pop(lexer));
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
