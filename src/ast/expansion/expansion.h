#ifndef EXPANSION_H
#define EXPANSION_H
#include <stddef.h>
#include <unistd.h>

#include "lexer/splitter.h"
#include "lexer/token.h"
#include "mbtstr/str.h"
#include "nodes/eval_ctx.h"
#include "streams/dstream.h"

struct mbt_str *expand_dollar(struct ast_eval_ctx *ctx, struct dstream *dstream,
                              int bracket);

struct mbt_str *expand(struct ast_eval_ctx *ctx, struct token *token);

#endif /* ! EXPANSION_H */
