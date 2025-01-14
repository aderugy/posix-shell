#include "lexer.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "splitter.h"
#include "token.h"
#include "utils/logger.h"

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
    "TOKEN_REDIR_FOPEN_RW"
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
                                           { ">", TOKEN_REDIR_STDOUT_FILE },
                                           { "<", TOKEN_REDIR_FILE_STDIN },
                                           { ">>", TOKEN_REDIR_STDOUT_FILE_A },
                                           { ">&", TOKEN_REDIR_STDOUT_FD },
                                           { "<&", TOKEN_REDIR_STDIN_FD },
                                           { ">|",
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

    //logger("lexer.c : will peek a shard\n");
    struct shard *shard = splitter_next(lexer->stream);
    if (!shard)
    {
        logger("not shard\n");
        token->type = TOKEN_EOF;
        return token;
    }

    //logger("lexer.c : peeked a shard\n");

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
    // logger("PEEKED TOKEN: %s\n", get_token_name(lexer->next->type));
    // if (lexer->next->value.c)
    // logger("\tValue: %s\n", lexer->next->value.c);

    return lexer->next;
}

struct token *lexer_pop(struct lexer *lexer)
{
    if (!lexer->stream)
    {
        return NULL;
    }

    struct token *token = lexer->next ? lexer->next : lex(lexer);
    // logger("lexer.c: found token %s\n", get_token_name(token->type));
    lexer->next = token->type != TOKEN_EOF ? lex(lexer) : NULL;
    /*if (lexer->next)
    {
         logger("lexer.c: next token is set with %s\n",
               get_token_name(lexer->next->type));
    }*/
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
