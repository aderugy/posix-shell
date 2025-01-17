#ifndef EXPANSION_H
#define EXPANSION_H
#include <stddef.h>
#include <unistd.h>

#include "lexer/splitter.h"
#include "lexer/token.h"
#include "mbtstr/str.h"
#include "streams/dstream.h"

/*
void expand_dollar(struct dstream *dstream);

void expand(struct mbt_str *str, struct dstream* dstream, int *brackets);
struct mbt_str *expand_word(struct token *token);

struct mbt_str *get_param(struct mbt_str *str);
*/

void assignment(char *data);

#endif /* ! EXPANSION_H */
