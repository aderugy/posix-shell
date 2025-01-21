#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    TOKEN_SEMICOLON, // ; [5]
    TOKEN_NEW_LINE, // \n [6]
    TOKEN_QUOTE, // ' [7]
    TOKEN_WORD, // word [8]
    TOKEN_PIPE, // | [9]
    TOKEN_NOT, // ! [10]
    TOKEN_EOF, // end of input marker [11]
    TOKEN_ERROR, // it is not a real token, it is returned in case of invalid
                 // input [12]
    TOKEN_AND, // and [18]
    TOKEN_OR, // or [19]
    TOKEN_REDIR_STDOUT_FILE,
    TOKEN_REDIR_FILE_STDIN,
    TOKEN_REDIR_STDOUT_FILE_A,
    TOKEN_REDIR_STDOUT_FD,
    TOKEN_REDIR_STDIN_FD,
    TOKEN_REDIR_STDOUT_FILE_NOTRUNC,
    TOKEN_REDIR_FOPEN_RW,
    TOKEN_COMPLEX_WORD,
    TOKEN_SUBSHELL,
    TOKEN_VARIABLE,
    TOKEN_ARITH,
    TOKEN_GLOBBING_STAR,
    TOKEN_GLOBBING_QM, // Questionmark '?'
};

struct token
{
    enum token_type type; // The kind of token
    union
    {
        char *c;
        ssize_t s;
    } value; // value of the token if neccessary

    char *state; // @DEPRECATED
    struct token *next;
};
#endif /* !TOKEN_H */
