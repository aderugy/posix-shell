#ifndef NODE_H
#define NODE_H

#include "builtins/builtins.h"
#include "eval_ctx.h"
#include "lexer/lexer.h"
#include "lexer/splitter.h"
#include "utils/eval_output_structure.h"
#include "utils/linked_list.h"

#define AST_EVAL_SUCCESS 0
#define AST_EVAL_ERROR 1

#define TOKEN_OK (token && token->type == TOKEN_WORD && !token->next)

enum ast_type
{
    AST_SIMPLE_COMMAND = 0,
    AST_SHELL_COMMAND, // 1
    AST_PIPELINE, // 2
    AST_COMMAND, // 3
    AST_ELEMENT, // 4
    AST_AND_OR, // 5
    AST_CLIST, // 6
    AST_INPUT, // 7
    AST_LIST, // 8
    AST_IF, // 9
    AST_ELSE, // 10
    AST_WHILE, // 11
    AST_UNTIL, // 12
    AST_FOR, // 13
    AST_REDIRECTION, // 14
    AST_PREFIX, // 15
    AST_FUNDEC, // 16
    AST_COMPLEX_WORD
};

struct ast_node
{
    enum ast_type type;
    void *value;
};

struct ast_node_operations
{
    void *(*parse)(struct lexer *lexer);
    int (*eval)(void *node, struct linked_list *out, void *ctx);
    void (*free)(void *node);
    void (*print)(void *node);
};

struct ast_node *ast_create(struct lexer *lexer, enum ast_type type);
int ast_eval(struct ast_node *node, struct linked_list *out,
             struct ast_eval_ctx *ctx);
void ast_free(struct ast_node *node);
void ast_print(struct ast_node *node);

#endif // !NODE_H
