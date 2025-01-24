#define _POSIX_C_SOURCE 200809L

#include "token.h"

#include <stdlib.h>
#include <string.h>

#include "utils/logger.h"

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

bool token_is_valid_identifier(struct token *token)
{
    return token && token->type == TOKEN_WORD
        && token->quote_type == SHARD_UNQUOTED && !token->next;
}

bool token_is_valid_keyword(struct token *token, const char *str)
{
    return token_is_valid_identifier(token) && strcmp(token->value.c, str) == 0;
}
