#ifndef AST_CWORD_H
#define AST_CWORD_H

#include "eval_ctx.h"
#include "lexer/shard.h"
#include "lexer/token.h"
#include "utils/linked_list.h"

struct ast_cword
{
    char *data;
    enum token_type type;
    enum shard_quote_type quote_type;

    struct ast_cword *next;
    bool sh_stdout_silent;
};

struct ast_cword_out
{
    struct linked_list *out;
};

struct ast_cword *ast_parse_cword(struct lexer *lexer);
int ast_eval_cword(struct ast_cword *node, struct linked_list *out,
                   struct ast_eval_ctx *ctx);
void ast_free_cword(struct ast_cword *node);

void ast_print_cword(struct ast_cword *node);

struct ast_cword *ast_parse_cword_from_token(struct token *token,
                                             struct lexer *lexer);

#endif // !AST_CWORD_H
