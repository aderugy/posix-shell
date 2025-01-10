#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    TOKEN_IF, // if [0]
    TOKEN_THEN, // then [1]
    TOKEN_ELIF, // elif [2]
    TOKEN_ELSE, // else [3]
    TOKEN_FI, // fi [4]
    TOKEN_SEMICOLON, // ; [5]
    TOKEN_NEW_LINE, // \n [6]
    TOKEN_QUOTE, // ' [7]
    TOKEN_WORD, // word [8]
    TOKEN_PIPE, // | [9]
    TOKEN_NOT, // ! [10]
    TOKEN_EOF, // end of input marker [11]
    TOKEN_ERROR // it is not a real token, it is returned in case of invalid
                // input [12]
};

struct token
{
    enum token_type type; // The kind of token
    union
    {
        char *c;
        ssize_t s;
    } value; // value of the token if neccessary
};
#endif /* !TOKEN_H */
