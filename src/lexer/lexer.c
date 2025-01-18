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
    CHECK_MEMORY_ERROR(res);

    res->stream = stream;
    res->tokens = stack_init((void (*)(void *))token_free);
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

    // logger("lexer.c : will peek a shard\n");
    struct shard *shard = splitter_next(lexer->stream);
    if (!shard)
    {
        token->type = TOKEN_EOF;
        return token;
    }

    // logger("lexer.c : peeked a shard\n");

    int condition = !strchr(shard->data, SHARD_DOUBLE_QUOTED);
    condition = condition && !strchr(shard->data, SHARD_SINGLE_QUOTED);
    condition = condition && !strchr(shard->data, SHARD_BACKSLASH_QUOTED);
    for (size_t i = 0; i < KEYWORDS_LEN && condition; i++)
    {
        char *first_occurence_of_chevron =
            strpbrk(shard->data, KEYWORDS[i].name);

        if (first_occurence_of_chevron
            && (strcmp(first_occurence_of_chevron, KEYWORDS[i].name) == 0))
        {
            token->type = KEYWORDS[i].type;
            if (token->type > 9)
            {
                size_t s = first_occurence_of_chevron - shard->data;
                token->value.c = malloc((s + 1) * sizeof(char));
                strncpy(token->value.c, shard->data, s);
                token->value.c[s] = 0;
            }
            break;
        }
    }

    if (token->type == TOKEN_ERROR)
    {
        // DOES NOT HANDLE THE FOLLOWING EXAMPLE : echo a"="B
        char *pos = NULL;
        // If the data contains a '=' and it does not come first
        if ((pos = strchr(shard->data, '=')) && pos != shard->data)
        {
            int len = pos - shard->data;
            // If the '=' is unquoted
            if (shard->state[len] == SHARD_UNQUOTED)
            {
                // if the name is SCL compliant
                if (convention_check(shard->data, len))
                {
                    token->type = TOKEN_AWORD;
                }
                else
                {
                    errx(LEX_ERROR, "incorrect assignment name");
                }
            }
        }

        if (token->type == TOKEN_ERROR)
        {
            token->type = TOKEN_WORD;
        }

        token->value.c = strdup(shard->data);
        token->state = strdup(shard->state);
    }
    shard_free(shard);
    return token;
}

struct token *lexer_peek(struct lexer *lexer)
{
    if (!lexer || !lexer->stream)
    {
        return NULL;
    }

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
    if (!lexer->stream)
    {
        return NULL;
    }

    struct token *token =
        lexer->tokens->size ? stack_pop(lexer->tokens) : lex(lexer);

    if (token->type == TOKEN_EOF)
    {
        stream_close(lexer->stream);
        lexer->stream = NULL;
    }

    return token;
}
