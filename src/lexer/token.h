#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    TOKEN_IF, // if
    TOKEN_THEN, // then
    TOKEN_ELIF, // elif
    TOKEN_ELSE, // else
    TOKEN_FI, // fi
    TOKEN_SEMICOLON, // ;
    TOKEN_NEW_LINE, // \n
    TOKEN_QUOTE, // '
    TOKEN_WORD, // word
    TOKEN_EOF, // end of input marker
    TOKEN_ERROR // it is not a real token, it is returned in case of invalid
                // input
};

struct token
{
    enum token_type type; // The kind of token
    union
    {
        char *c;
        ssize_t s;
    } value; // value of the token if neccessary
    struct token *next;
};
#endif /* !TOKEN_H */
