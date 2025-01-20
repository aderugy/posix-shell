#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "splitter.h"
#include "token.h"
#include "utils/err_utils.h"
#include "utils/logger.h"
#include "utils/naming.h"
#include "utils/xalloc.h"

static const char *token_names[] = {
    "TOKEN_SEMICOLON", // ; [5]
    "TOKEN_NEW_LINE", // \n [6]
    "TOKEN_QUOTE", // ' [7]
    "TOKEN_WORD", // word [8]
    "TOKEN_PIPE", // | [9]
    "TOKEN_NOT", // ! [10]
    "TOKEN_EOF", // end of input marker [11]
    "TOKEN_ERROR", // it is not a real token, it is returned in case of invalid
                   // input [12]
    "TOKEN_AND", // and [18]
    "TOKEN_OR", // or [19]
    "TOKEN_REDIR_STDOUT_FILE", "TOKEN_REDIR_FILE_STDIN",
    "TOKEN_REDIR_STDOUT_FILE_A", "TOKEN_REDIR_STDOUT_FD",
    "TOKEN_REDIR_STDIN_FD", "TOKEN_REDIR_STDOUT_FILE_NOTRUNC",
    "TOKEN_REDIR_FOPEN_RW", "TOKEN_COMPLEX_WORD", "TOKEN_SUBSHELL",
    "TOKEN_VARIABLE", "TOKEN_ARITH", "TOKEN_GLOBBING_STAR", "TOKEN_GLOBBING_QM"
};

const char *get_token_name(enum token_type token)
{
    if (token >= 0 && token < sizeof(token_names) / sizeof(token_names[0]))
    {
        return token_names[token];
    }
    return "UNKNOWN_TOKEN";
}

static const struct keyword KEYWORDS[] = { { ";", TOKEN_SEMICOLON },
                                           { "\n", TOKEN_NEW_LINE },
                                           { "'", TOKEN_QUOTE },
                                           { "|", TOKEN_PIPE },
                                           { "&&", TOKEN_AND },
                                           { "||", TOKEN_OR },
                                           { "<>", TOKEN_REDIR_FOPEN_RW },
                                           { ">>", TOKEN_REDIR_STDOUT_FILE_A },
                                           { ">&", TOKEN_REDIR_STDOUT_FD },
                                           { "<&", TOKEN_REDIR_STDIN_FD },
                                           { ">|",
                                             TOKEN_REDIR_STDOUT_FILE_NOTRUNC },
                                           { ">", TOKEN_REDIR_STDOUT_FILE },
                                           { "<", TOKEN_REDIR_FILE_STDIN },
                                           { NULL, TOKEN_EOF } };

#define KEYWORDS_LEN (sizeof(KEYWORDS) / sizeof(KEYWORDS[0]) - 1)

static struct token *lexer_chain(struct lexer *lexer);

struct lexer *lexer_create(struct stream *stream)
{
    struct lexer *res = calloc(1, sizeof(struct lexer));
    CHECK_MEMORY_ERROR(res);

    res->tokens = stack_init((void (*)(void *))token_free);
    res->ctx = splitter_ctx_init(stream);
    return res;
}

void token_print(struct token *token)
{
    logger("%s '%s'\n", get_token_name(token->type),
           token->value.c ? token->value.c : "");

    if (token->next)
    {
        logger("CHAINED: ");
        token_print(token->next);
    }
}

void token_free(struct token *token)
{
    if (token)
    {
        if (token->next)
        {
            token_free(token->next);
        }

        if (token->value.c)
        {
            free(token->value.c);
        }

        if (token->state)
        {
            free(token->state);
        }

        free(token);
    }
}

void lexer_free(struct lexer *lexer)
{
    stack_free(lexer->tokens);
    if (lexer->ctx)
    {
        splitter_ctx_free(lexer->ctx);
    }

    free(lexer);
}

static struct token *lex(struct lexer *lexer)
{
    struct token *token = xcalloc(1, sizeof(struct token));
    token->type = TOKEN_ERROR;

    struct shard *shard = splitter_pop(lexer->ctx);
    if (!shard)
    {
        if (lexer->ctx->err)
        {
            lexer_error(lexer, NULL);
        }

        goto error;
    }

    if (shard->can_chain)
    {
        token->next = lexer_chain(lexer);
    }

    switch (shard->type)
    {
    case SHARD_WORD:
    case SHARD_OPERATOR:
        if (shard->quote_type == SHARD_UNQUOTED && !token->next)
        {
            for (size_t i = 0; i < KEYWORDS_LEN; i++)
            {
                if (strcmp(KEYWORDS[i].name, shard->data) == 0)
                {
                    token->type = KEYWORDS[i].type;
                    break;
                }
            }

            if (token->type == TOKEN_ERROR)
            {
                token->type = TOKEN_WORD;
                token->value.c = strdup(shard->data);
            }
        }
        else
        {
            token->type = TOKEN_COMPLEX_WORD;
            token->value.c = strdup(shard->data);
        }
        break;
    case SHARD_EXPANSION_VARIABLE:
        token->type = TOKEN_VARIABLE;
        token->value.c = strdup(shard->data);
        break;
    case SHARD_EXPANSION_ARITH:
        token->type = TOKEN_ARITH;
        token->value.c = strdup(shard->data);
        break;
    case SHARD_EXPANSION_SUBSHELL:
        token->type = TOKEN_SUBSHELL;
        token->value.c = strdup(shard->data);
        break;
    case SHARD_GLOBBING_STAR:
        token->type = TOKEN_GLOBBING_STAR;
        break;
    case SHARD_GLOBBING_QUESTIONMARK:
        token->type = TOKEN_GLOBBING_QM;
        break;
    case SHARD_DELIMITER:
        token_free(token);
        shard_free(shard);
        return lex(lexer);
    }

    shard_free(shard);
    return token;

error:
    token_free(token);
    shard_free(shard);
    return NULL;
}

static struct token *lexer_chain(struct lexer *lexer)
{
    struct shard *shard = splitter_peek(lexer->ctx);
    if (!shard)
    {
        return NULL;
    }

    if (!shard->can_chain)
    {
        return NULL;
    }

    return lex(lexer);
}

struct token *lexer_peek(struct lexer *lexer)
{
    struct token *token = stack_peek(lexer->tokens);
    if (!token)
    {
        token = lex(lexer);

        if (token)
        {
            stack_push(lexer->tokens, token);
        }
    }

    return token;
}

struct token *lexer_pop(struct lexer *lexer)
{
    struct token *token =
        lexer->tokens->size ? stack_pop(lexer->tokens) : lex(lexer);

    return token;
}

void lexer_error(struct lexer *lexer, const char *msg)
{
    if (msg)
    {
        warnx("Syntax error: %s", msg);
    }

    lexer->error = true;
    stream_empty(lexer->ctx->stream);
}
