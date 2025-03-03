#include "lexer.h"

#include <ctype.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shard.h"
#include "splitter.h"
#include "token.h"
#include "utils/err_utils.h"
#include "utils/logger.h"
#include "utils/naming.h"
#include "utils/xalloc.h"

static const struct keyword KEYWORDS[] = {
    { "<>", TOKEN_REDIR_FOPEN_RW },
    { ">>", TOKEN_REDIR_STDOUT_FILE_A },
    { ">&", TOKEN_REDIR_STDOUT_FD },
    { "<&", TOKEN_REDIR_STDIN_FD },
    { ">|", TOKEN_REDIR_STDOUT_FILE_NOTRUNC },
    { ">", TOKEN_REDIR_STDOUT_FILE },
    { "<", TOKEN_REDIR_FILE_STDIN },
    { ";", TOKEN_SEMICOLON },
    { "\n", TOKEN_NEW_LINE },
    { "'", TOKEN_QUOTE },
    { "|", TOKEN_PIPE },
    { "&&", TOKEN_AND },
    { "||", TOKEN_OR },
    { "<>", TOKEN_REDIR_FOPEN_RW },
    { ">>", TOKEN_REDIR_STDOUT_FILE_A },
    { ">&", TOKEN_REDIR_STDOUT_FD },
    { "<&", TOKEN_REDIR_STDIN_FD },
    { ">|", TOKEN_REDIR_STDOUT_FILE_NOTRUNC },
    { ">", TOKEN_REDIR_STDOUT_FILE },
    { "<", TOKEN_REDIR_FILE_STDIN },
    { "(", TOKEN_LEFT_PARENTHESIS },
    { ")", TOKEN_RIGHT_PARENTHESIS },
    { "{", TOKEN_LEFT_BRACKET },
    { "}", TOKEN_RIGHT_BRACKET },
    { NULL, TOKEN_EOF }
};

#define KEYWORDS_LEN (sizeof(KEYWORDS) / sizeof(KEYWORDS[0]) - 1)

static struct token *lex(struct lexer *lexer, bool nullable);
static struct token *lexer_chain(struct lexer *lexer);

struct lexer *lexer_create(struct stream *stream)
{
    struct lexer *res = xcalloc(1, sizeof(struct lexer));
    res->tokens = stack_init((void (*)(void *))token_free);
    res->ctx = splitter_ctx_init(stream);
    return res;
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

static int token_get_keyword_type(char *str)
{
    for (size_t i = 0; i < KEYWORDS_LEN; i++)
    {
        if (strcmp(KEYWORDS[i].name, str) == 0)
        {
            return KEYWORDS[i].type;
        }
    }

    return TOKEN_ERROR;
}

static bool lexer_handle_operator(struct lexer *lexer, struct shard *shard,
                                  struct token *token)
{
    struct shard *next;
    if (shard_is_operator(shard, "("))
    {
        splitter_ctx_expect(lexer->ctx,
                            SHARD_CONTEXT_EXPANSION_POPPED_PARENTHESIS);

        next = splitter_peek(lexer->ctx);
        if (next && next->type == SHARD_EXPANSION_SUBSHELL && !*(next->data))
        {
            shard_free(splitter_pop(lexer->ctx));
            token->type = TOKEN_LEFT_PARENTHESIS;
        }
        else
        {
            struct shard *shard_two = splitter_pop(lexer->ctx);
            if (!shard_two || !splitter_peek(lexer->ctx))
            {
                logger("returning true for parenthesis\n");
                shard_free(shard_two);
                return true;
            }

            shard_free(shard);
            shard_free(splitter_pop(lexer->ctx));
            token->type = TOKEN_SUBSHELL;
            token->sh_stdout_silent = true;
        }

        return false;
    }

    if (shard->quote_type == SHARD_UNQUOTED && !token->next)
    {
        token->type = token_get_keyword_type(shard->data);

        if (token->type == TOKEN_ERROR)
        {
            token->type = TOKEN_WORD;
        }
    }
    else
    {
        token->type = TOKEN_WORD;
    }

    return false;
}

static struct token *lexer_handle_delimiters(struct lexer *lexer,
                                             struct token *token,
                                             struct shard *shard, bool nullable)
{
    token_free(token);
    shard_free(shard);

    shard = splitter_peek(lexer->ctx);
    if (nullable && (!shard || !shard->can_chain))
    {
        return NULL;
    }

    return lex(lexer, nullable);
}

static void lexer_get_token_type(struct token *token, struct shard *shard)
{
    switch (shard->type)
    {
    case SHARD_EXPANSION_VARIABLE:
        token->type = TOKEN_VARIABLE;
        break;
    case SHARD_EXPANSION_ARITH:
        token->type = TOKEN_ARITH;
        break;
    case SHARD_EXPANSION_SUBSHELL:
        token->type = TOKEN_SUBSHELL;
        break;
    case SHARD_GLOBBING_STAR:
        token->type = TOKEN_GLOBBING_STAR;
        break;
    case SHARD_GLOBBING_QUESTIONMARK:
        token->type = TOKEN_GLOBBING_QM;
        break;
    default:
        break;
    }
}

#define OFFSET(shard) isdigit(*shard->data) ? 1 : 0
static struct token *lex(struct lexer *lexer, bool nullable)
{
    struct token *token = xcalloc(1, sizeof(struct token));
    token->type = TOKEN_ERROR;

    struct shard *shard = splitter_pop(lexer->ctx);
    if (!shard)
    {
        if (lexer->ctx->err)
        {
            goto error;
        }

        token->type = TOKEN_EOF;
        lexer->eof = true;
        return token;
    }

    token->quote_type = shard->quote_type;
    switch (shard->type)
    {
    case SHARD_REDIR:
        token->type = token_get_keyword_type(shard->data + (OFFSET(shard)));
        break;

    case SHARD_WORD:
    case SHARD_OPERATOR:
        if (lexer_handle_operator(lexer, shard, token))
        {
            goto error;
        }
        break;
    case SHARD_DELIMITER:
        return lexer_handle_delimiters(lexer, token, shard, nullable);
    default:
        lexer_get_token_type(token, shard);
        break;
    }

    if (token->type == TOKEN_ERROR)
    {
        goto error;
    }

    token->value.c = strdup(shard->data);
    if (shard->can_chain)
    {
        token->next = lexer_chain(lexer);
    }
    shard_free(shard);

    return token;

error:
    token_free(token);
    if (shard)
    {
        shard_free(shard);
    }
    lexer_error(lexer, "erreur inconnue");
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

    return lex(lexer, true);
}

struct token *lexer_peek(struct lexer *lexer)
{
    if (lexer->error)
    {
        return NULL;
    }

    struct token *token = stack_peek(lexer->tokens);
    if (!token && !lexer->eof)
    {
        token = lex(lexer, false);

        if (token)
        {
            stack_push(lexer->tokens, token);
        }
    }

    return token;
}

struct token *lexer_peek_two(struct lexer *lexer)
{
    if (lexer->error)
    {
        return NULL;
    }

    struct token *first = lexer_pop(lexer);
    if (!first)
    {
        return NULL;
    }

    struct token *second = lexer_pop(lexer);
    if (!second)
    {
        stack_push(lexer->tokens, first);
        return NULL;
    }

    stack_push(lexer->tokens, second);
    stack_push(lexer->tokens, first);

    return second;
}

struct token *lexer_pop(struct lexer *lexer)
{
    if (lexer->error || lexer->eof)
    {
        return lexer->tokens->size ? stack_pop(lexer->tokens) : NULL;
    }

    struct token *token =
        lexer->tokens->size ? stack_pop(lexer->tokens) : lex(lexer, false);

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

    struct token *token;
    while ((token = stack_pop(lexer->tokens)))
    {
        token_free(token);
    }
}
