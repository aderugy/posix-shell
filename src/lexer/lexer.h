#ifndef LEXER_H
#define LEXER_H

#include "splitter.h"
#include "streams/streams.h"
#include "token.h"
#include "utils/stack.h"

#define LEX_ERROR 2

#define LEX_ANY 0
#define LEX_OPERATOR 1
#define LEX_UNQUOTED 2
#define LEX_UNCHAINED 4

#define LEX_PLAIN_WORD LEX_UNQUOTED &LEX_UNCHAINED
#define LEX_WORD 0

/**
 * \page Lexer
 *
 * The lexer cuts some input text into blocks called tokens.

 * This process is done **on demand**: the lexer doesn't read the
 * input more than it needs, only creates tokens when lexer_peek
 * or lexer_pop is called, and no token is available.
 */

struct lexer
{
    struct stack *tokens;

    struct splitter_ctx *ctx;

    bool error;
    bool eof;
};

struct keyword
{
    const char *name;
    enum token_type type;
};

void token_print(struct token *token);

/**
 * \brief Creates a new lexer given an input string.
 */
struct lexer *lexer_create(struct stream *stream);

/**
 ** \brief Frees the given lexer, but not its input.
 */
void lexer_free(struct lexer *lexer);

/**
 * \brief Returns the next token, but doesn't move forward: calling lexer_peek
 * multiple times in a row always returns the same result.
 * This function is meant to help the parser check if the next token matches
 * some rule.
 */
struct token *lexer_peek(struct lexer *lexer);

/**
 * \brief Returns the next token, and removes it from the stream:
 *   calling lexer_pop in a loop will iterate over all tokens until EOF.
 */
struct token *lexer_pop(struct lexer *lexer);

void lexer_error(struct lexer *lexer, const char *msg);

const char *get_token_name(enum token_type token);

void token_free(struct token *token);
#endif /* !LEXER_H */
