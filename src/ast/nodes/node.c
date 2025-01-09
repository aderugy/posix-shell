#include "node.h"

#include <err.h>
#include <stdlib.h>

#include "and_or.h"
#include "clist.h"
#include "command.h"
#include "element.h"
#include "else.h"
#include "if.h"
#include "input.h"
#include "list.h"
#include "pipeline.h"
#include "shell_command.h"
#include "simple_command.h"
#include "utils/logger.h"

static const struct ast_node_operations AST_FN[] = {
    { (void *(*)(struct lexer *))ast_parse_simple_cmd,
      (int (*)(void *, void **))ast_eval_simple_cmd,
      (void (*)(void *))ast_free_simple_cmd,
      (void (*)(void *))ast_print_simple_cmd },

    { (void *(*)(struct lexer *))ast_parse_shell_cmd,
      (int (*)(void *, void **))ast_eval_shell_cmd,
      (void (*)(void *))ast_free_shell_cmd,
      (void (*)(void *))ast_print_shell_cmd },

    { (void *(*)(struct lexer *))ast_parse_pipeline,
      (int (*)(void *, void **))ast_eval_pipeline,
      (void (*)(void *))ast_free_pipeline,
      (void (*)(void *))ast_print_pipeline },

    { (void *(*)(struct lexer *))ast_parse_cmd,
      (int (*)(void *, void **))ast_eval_cmd, (void (*)(void *))ast_free_cmd,
      (void (*)(void *))ast_print_cmd },

    { (void *(*)(struct lexer *))ast_parse_element,
      (int (*)(void *, void **))ast_eval_element,
      (void (*)(void *))ast_free_element, (void (*)(void *))ast_print_element },

    { (void *(*)(struct lexer *))ast_parse_and_or,
      (int (*)(void *, void **))ast_eval_and_or,
      (void (*)(void *))ast_free_and_or, (void (*)(void *))ast_print_and_or },

    { (void *(*)(struct lexer *))ast_parse_clist,
      (int (*)(void *, void **))ast_eval_clist,
      (void (*)(void *))ast_free_clist, (void (*)(void *))ast_print_clist },

    { (void *(*)(struct lexer *))ast_parse_input,
      (int (*)(void *, void **))ast_eval_input,
      (void (*)(void *))ast_free_input, (void (*)(void *))ast_print_input },

    { (void *(*)(struct lexer *))ast_parse_list,
      (int (*)(void *, void **))ast_eval_list, (void (*)(void *))ast_free_list,
      (void (*)(void *))ast_print_list },

    { (void *(*)(struct lexer *))ast_parse_if,
      (int (*)(void *, void **))ast_eval_if, (void (*)(void *))ast_free_if,
      (void (*)(void *))ast_print_if },

    { (void *(*)(struct lexer *))ast_parse_else,
      (int (*)(void *, void **))ast_eval_else, (void (*)(void *))ast_free_else,
      (void (*)(void *))ast_print_else }
};

struct ast_node *ast_create(struct lexer *lexer, enum ast_type type)
{
    if (!lexer)
    {
        errx(EXIT_FAILURE, "lexer is NULL");
    }
    void *value = AST_FN[type].parse(lexer);
    if (!value)
    {
        return NULL;
    }

    struct ast_node *root = calloc(1, sizeof(struct ast_node));
    if (!root)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    root->type = type;
    root->value = value;
    return root;
}

int ast_eval(struct ast_node *node, void **out)
{
    return AST_FN[node->type].eval(node->value, out);
}

void ast_free(struct ast_node *node)
{
    logger("%i\n", node->type);
    AST_FN[node->type].free(node->value);
    free(node);
}

void ast_print(struct ast_node *node)
{
    AST_FN[node->type].print(node->value);
}
