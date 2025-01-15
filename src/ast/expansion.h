#ifndef EXPANSION_H
#define EXPANSION_H
#include <stddef.h>
#include <unistd.h>

#include "lexer/splitter.h"
#include "lexer/token.h"
#include "mbtstr/str.h"

char *expand_word(struct token *token);

#endif /* ! EXPANSION_H */
