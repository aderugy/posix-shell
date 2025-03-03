#include "node.h"

#include <err.h>
#include <stdlib.h>

#include "and_or.h"
#include "ast/ast.h"
#include "clist.h"
#include "command.h"
#include "element.h"
#include "else.h"
#include "for.h"
#include "fundec.h"
#include "if.h"
#include "input.h"
#include "list.h"
#include "pipeline.h"
#include "prefix.h"
#include "redirection.h"
#include "shell_command.h"
#include "simple_command.h"
#include "until.h"
#include "utils/logger.h"
#include "while.h"
extern int logger_stack_idx;
static const struct ast_node_operations AST_FN[] = {
    { (void *(*)(struct lexer *))ast_parse_simple_cmd,
      (int (*)(void *, struct linked_list *, void *))ast_eval_simple_cmd,
      (void (*)(void *))ast_free_simple_cmd,
      (void (*)(void *))ast_print_simple_cmd },

    { (void *(*)(struct lexer *))ast_parse_shell_cmd,
      (int (*)(void *, struct linked_list *, void *))ast_eval_shell_cmd,
      (void (*)(void *))ast_free_shell_cmd,
      (void (*)(void *))ast_print_shell_cmd },

    { (void *(*)(struct lexer *))ast_parse_pipeline,
      (int (*)(void *, struct linked_list *, void *))ast_eval_pipeline,
      (void (*)(void *))ast_free_pipeline,
      (void (*)(void *))ast_print_pipeline },

    { (void *(*)(struct lexer *))ast_parse_cmd,
      (int (*)(void *, struct linked_list *, void *))ast_eval_cmd,
      (void (*)(void *))ast_free_cmd, (void (*)(void *))ast_print_cmd },

    { (void *(*)(struct lexer *))ast_parse_element,
      (int (*)(void *, struct linked_list *, void *))ast_eval_element,
      (void (*)(void *))ast_free_element, (void (*)(void *))ast_print_element },

    { (void *(*)(struct lexer *))ast_parse_and_or,
      (int (*)(void *, struct linked_list *, void *))ast_eval_and_or,
      (void (*)(void *))ast_free_and_or, (void (*)(void *))ast_print_and_or },

    { (void *(*)(struct lexer *))ast_parse_clist,
      (int (*)(void *, struct linked_list *, void *))ast_eval_clist,
      (void (*)(void *))ast_free_clist, (void (*)(void *))ast_print_clist },

    { (void *(*)(struct lexer *))ast_parse_input,
      (int (*)(void *, struct linked_list *, void *))ast_eval_input,
      (void (*)(void *))ast_free_input, (void (*)(void *))ast_print_input },

    { (void *(*)(struct lexer *))ast_parse_list,
      (int (*)(void *, struct linked_list *, void *))ast_eval_list,
      (void (*)(void *))ast_free_list, (void (*)(void *))ast_print_list },

    { (void *(*)(struct lexer *))ast_parse_if,
      (int (*)(void *, struct linked_list *, void *))ast_eval_if,
      (void (*)(void *))ast_free_if, (void (*)(void *))ast_print_if },

    { (void *(*)(struct lexer *))ast_parse_else,
      (int (*)(void *, struct linked_list *, void *))ast_eval_else,
      (void (*)(void *))ast_free_else, (void (*)(void *))ast_print_else },

    { (void *(*)(struct lexer *))ast_parse_while,
      (int (*)(void *, struct linked_list *, void *))ast_eval_while,
      (void (*)(void *))ast_free_while, (void (*)(void *))ast_print_while },

    { (void *(*)(struct lexer *))ast_parse_until,
      (int (*)(void *, struct linked_list *, void *))ast_eval_until,
      (void (*)(void *))ast_free_until, (void (*)(void *))ast_print_until },

    { (void *(*)(struct lexer *))ast_parse_for,
      (int (*)(void *, struct linked_list *, void *))ast_eval_for,
      (void (*)(void *))ast_free_for, (void (*)(void *))ast_print_for },

    { (void *(*)(struct lexer *))ast_parse_redir,
      (int (*)(void *, struct linked_list *, void *))ast_eval_redir,
      (void (*)(void *))ast_free_redir, (void (*)(void *))ast_print_redir },

    { (void *(*)(struct lexer *))ast_parse_prefix,
      (int (*)(void *, struct linked_list *, void *))ast_eval_prefix,
      (void (*)(void *))ast_free_prefix, (void (*)(void *))ast_print_prefix },

    { (void *(*)(struct lexer *))ast_parse_fundec,
      (int (*)(void *, struct linked_list *, void *))ast_eval_fundec,
      (void (*)(void *))ast_free_fundec, (void (*)(void *))ast_print_fundec },

    { (void *(*)(struct lexer *))ast_parse_cword,
      (int (*)(void *, struct linked_list *, void *))ast_eval_cword,
      (void (*)(void *))ast_free_cword, (void (*)(void *))ast_print_cword },
};
struct ast_node *ast_create(struct lexer *lexer, enum ast_type type)
{
    logger_stack_idx++;
    if (!lexer)
    {
        // lexer_parse_error actually
        errx(AST_PARSE_ERROR, "lexer is NULL");
    }
    void *value = AST_FN[type].parse(lexer);
    if (!value)
    {
        logger_stack_idx--;
        return NULL;
    }

    struct ast_node *root = calloc(1, sizeof(struct ast_node));
    if (!root)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    root->type = type;
    root->value = value;
    logger_stack_idx--;
    return root;
}

int ast_eval(struct ast_node *node, struct linked_list *out,
             __attribute((unused)) struct ast_eval_ctx *ctx)
{
    if (!node)
    {
        errx(AST_PARSE_ERROR, "eval NULL");
    }

    return AST_FN[node->type].eval(node->value, out, ctx);
}

void ast_free(struct ast_node *node)
{
    if (!node)
    {
        return;
    }

    AST_FN[node->type].free(node->value);
    free(node);
}

void ast_print(struct ast_node *node)
{
    if (!node)
    {
        logger("(nil)");
        return;
    }

    AST_FN[node->type].print(node->value);
}
