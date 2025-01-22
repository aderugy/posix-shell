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

static const char *token_names[] = {
    "TOKEN_SEMICOLON", // ;
    "TOKEN_NEW_LINE", // \n
    "TOKEN_QUOTE", // '
    "TOKEN_WORD", // word
    "TOKEN_PIPE", // |
    "TOKEN_NOT", // !
    "TOKEN_EOF", // end of input marker
    "TOKEN_ERROR", // it is not a real token, it is returned in case of invalid
    "TOKEN_AND", // and
    "TOKEN_OR", // or
    "TOKEN_REDIR_STDOUT_FILE",
    "TOKEN_REDIR_FILE_STDIN",
    "TOKEN_REDIR_STDOUT_FILE_A",
    "TOKEN_REDIR_STDOUT_FD",
    "TOKEN_REDIR_STDIN_FD",
    "TOKEN_REDIR_STDOUT_FILE_NOTRUNC",
    "TOKEN_REDIR_FOPEN_RW",
    "TOKEN_SUBSHELL",
    "TOKEN_VARIABLE",
    "TOKEN_ARITH",
    "TOKEN_GLOBBING_STAR",
    "TOKEN_GLOBBING_QM",
    "TOKEN_LEFT_PARENTHESIS",
    "TOKEN_RIGHT_PARENTHESIS",
    "TOKEN_LEFT_BRACKET",
    "TOKEN_RIGHT_BRACKET",
};

const char *get_token_name(enum token_type token)
{
    if (token >= 0 && token < sizeof(token_names) / sizeof(token_names[0]))
    {
        return token_names[token];
    }
    return "UNKNOWN_TOKEN";
}

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

static struct token *lexer_chain(struct lexer *lexer);

struct lexer *lexer_create(struct stream *stream)
{
    struct lexer *res = xcalloc(1, sizeof(struct lexer));
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

bool token_is_valid_identifier(struct token *token)
{
    return token && token->type == TOKEN_WORD
        && token->quote_type == SHARD_UNQUOTED && !token->next;
}

bool token_is_valid_keyword(struct token *token, const char *str)
{
    return token_is_valid_identifier(token) && strcmp(token->value.c, str) == 0;
}

static struct token *lex(struct lexer *lexer, bool nullable)
{
    if (lexer->eof)
    {
        return NULL;
    }

    struct token *token = xcalloc(1, sizeof(struct token));
    token->type = TOKEN_ERROR;

    struct shard *shard = splitter_pop(lexer->ctx);
    if (!shard)
    {
        if (lexer->ctx->err)
        {
            lexer_error(lexer, NULL);
            goto error;
        }

        token->type = TOKEN_EOF;
        lexer->eof = true;
        return token;
    }

    token->quote_type = shard->quote_type;
    struct shard *next;
    switch (shard->type)
    {
    case SHARD_REDIR:
        token->type = token_get_keyword_type(shard->data
                                             + (isdigit(*shard->data) ? 1 : 0));
        break;

    case SHARD_WORD:
    case SHARD_OPERATOR:
        if (!*shard->data)
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

        if (shard_is_operator(shard, "("))
        {
            splitter_ctx_expect(lexer->ctx,
                                SHARD_CONTEXT_EXPANSION_POPPED_PARENTHESIS);

            next = splitter_peek(lexer->ctx);
            if (next && next->type == SHARD_EXPANSION_SUBSHELL && !*next->data)
            {
                shard_free(splitter_pop(lexer->ctx));
                token->type = TOKEN_LEFT_PARENTHESIS;
            }
            else
            {
                shard_free(shard);
                shard = splitter_pop(lexer->ctx);
                if (!shard || !splitter_peek(lexer->ctx))
                {
                    goto error;
                }
                shard_free(splitter_pop(lexer->ctx));
                token->type = TOKEN_SUBSHELL;
            }

            break;
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
        break;
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
    case SHARD_DELIMITER:
        token_free(token);
        shard_free(shard);

        shard = splitter_peek(lexer->ctx);
        if (nullable && (!shard || !shard->can_chain))
        {
            return NULL;
        }

        return lex(lexer, nullable);
    }

    token->value.c = strdup(shard->data);
    if (token->type == TOKEN_ERROR)
    {
        goto error;
    }

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
