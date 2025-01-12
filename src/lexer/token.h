#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    TOKEN_THEN, // then [1]
    TOKEN_ELIF, // elif [2]
    TOKEN_SEMICOLON, // ; [5]
    TOKEN_NEW_LINE, // \n [6]
    TOKEN_QUOTE, // ' [7]
    TOKEN_WORD, // word [8]
    TOKEN_PIPE, // | [9]
    TOKEN_NOT, // ! [10]
    TOKEN_EOF, // end of input marker [11]
    TOKEN_ERROR, // it is not a real token, it is returned in case of invalid
                 // input [12]
    TOKEN_WHILE, // while [13]
    TOKEN_UNTIL, // until [14]
    TOKEN_FOR, // until [15]
    TOKEN_DO, // do [16]
    TOKEN_DONE, // done [17]
    TOKEN_AND, // and [18]
    TOKEN_OR, // or [19]
    TOKEN_REDIR_STDOUT_FILE,
    TOKEN_REDIR_FILE_STDIN,
    TOKEN_REDIR_STDOUT_FILE_A,
    TOKEN_REDIR_STDOUT_FD,
    TOKEN_REDIR_STDIN_FD,
    TOKEN_REDIR_STDOUT_FILE_NOTRUNC,
    TOKEN_REDIR_FOPEN_RW
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
