#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "splitter.h"
#include "token.h"
#include "utils/logger.h"

static const char *token_names[] = {
  "TOKEN_SEMICOLON", "TOKEN_NEW_LINE", "TOKEN_QUOTE",
    "TOKEN_WORD",  "TOKEN_PIPE",      "TOKEN_NOT",      "TOKEN_EOF",
    "TOKEN_ERROR", "TOKEN_WHILE",     "TOKEN_UNTIL",    "TOKEN_FOR",
    "TOKEN_DO",    "TOKEN_DONE"
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
                                           { ";", TOKEN_SEMICOLON },
                                           { "\n", TOKEN_NEW_LINE },
                                           { "'", TOKEN_QUOTE },
                                           { "|", TOKEN_PIPE },
                                           { "while", TOKEN_WHILE },
                                           { "until", TOKEN_UNTIL },
                                           { "for", TOKEN_FOR },
                                           { "do", TOKEN_DO },
                                           { "done", TOKEN_DONE },
                                           { "&&", TOKEN_AND },
                                           { "||", TOKEN_OR },
                                           { ">", TOKEN_REDIR_STDOUT_FILE },
                                           { "<", TOKEN_REDIR_FILE_STDIN },
                                           { ">>", TOKEN_REDIR_STDOUT_FILE_A },
                                           { ">&", TOKEN_REDIR_STDOUT_FD },
                                           { "<&", TOKEN_REDIR_STDIN_FD },
                                           { ">,",
                                             TOKEN_REDIR_STDOUT_FILE_NOTRUNC },
                                           { "<>", TOKEN_REDIR_FOPEN_RW },
                                           { NULL, TOKEN_EOF } };

#define KEYWORDS_LEN (sizeof(KEYWORDS) / sizeof(KEYWORDS[0]) - 1)

struct lexer *lexer_create(struct stream *stream)
{
    struct lexer *res = calloc(1, sizeof(struct lexer));
    if (!res)
    {
        return NULL;
    }

    res->stream = stream;
    return res;
}

void token_free(struct token *token)
{
    if (token)
    {
        if (token->value.c)
        {
            free(token->value.c);
        }

        free(token);
    }
}

void lexer_free(struct lexer *lexer)
{
    token_free(lexer->next);
    if (lexer->stream)
    {
        stream_close(lexer->stream);
    }
    free(lexer);
}

static struct token *lex(struct lexer *lexer)
{
    struct token *token = calloc(1, sizeof(struct token));
    if (!token)
    {
        errx(EXIT_FAILURE, "lex: memory error");
    }
    token->type = TOKEN_ERROR;

    struct shard *shard = splitter_next(lexer->stream);
    if (!shard)
    {
        logger("not shard\n");
        token->type = TOKEN_EOF;
        return token;
    }

    for (size_t i = 0; i < KEYWORDS_LEN && shard->quoted == SHARD_UNQUOTED; i++)
    {
        if (strcmp(shard->data, KEYWORDS[i].name) == 0)
        {
            token->type = KEYWORDS[i].type;
            break;
        }
    }

    if (token->type == TOKEN_ERROR)
    {
        token->type = TOKEN_WORD;
        token->value.c = strdup(shard->data);
        logger("--LEXER.C: lexed : %s\n", token->value.c);
    }

    shard_free(shard);
    return token;
}

struct token *lexer_peek(struct lexer *lexer)
{
    if (!lexer->stream)
    {
        return NULL;
    }

    if (!lexer->next)
    {
        lexer->next = lex(lexer);
    }

    return lexer->next;
}

struct token *lexer_pop(struct lexer *lexer)
{
    if (!lexer->stream)
    {
        return NULL;
    }

    struct token *token = lexer->next ? lexer->next : lex(lexer);
    lexer->next = token->type != TOKEN_EOF ? lex(lexer) : NULL;

    if (token->type == TOKEN_EOF)
    {
        stream_close(lexer->stream);
        lexer->stream = NULL;
    }
    logger("POPED TOKEN: %s\n", get_token_name(token->type));
    if (token->value.c)
        logger("\tValue: %s\n", token->value.c);

    return token;
}
