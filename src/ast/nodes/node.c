#include "node.h"

#include <err.h>
#include <stdlib.h>

#include "and_or.h"
#include "clist.h"
#include "command.h"
#include "element.h"
#include "if.h"
#include "input.h"
#include "list.h"
#include "pipeline.h"
#include "shell_command.h"
#include "simple_command.h"
#include "utils/logger.h"

static const struct ast_node_operations AST_FN[] = {
    { (void *)ast_parse_simple_cmd, (void *)ast_eval_simple_cmd,
      (void *)ast_free_simple_cmd, (void *)ast_print_simple_cmd },

    { (void *)ast_parse_shell_cmd, (void *)ast_eval_shell_cmd,
      (void *)ast_free_shell_cmd, (void *)ast_print_shell_cmd },

    { (void *)ast_parse_pipeline, (void *)ast_eval_pipeline,
      (void *)ast_free_pipeline, (void *)ast_print_pipeline },

    { (void *)ast_parse_cmd, (void *)ast_eval_cmd, (void *)ast_free_cmd,
      (void *)ast_print_cmd },

    { (void *)ast_parse_element, (void *)ast_eval_element,
      (void *)ast_free_element, (void *)ast_print_element },

    { (void *)ast_parse_and_or, (void *)ast_eval_and_or,
      (void *)ast_free_and_or, (void *)ast_print_and_or },

    { (void *)ast_parse_clist, (void *)ast_eval_clist, (void *)ast_free_clist,
      (void *)ast_print_clist },

    { (void *)ast_parse_input, (void *)ast_eval_input, (void *)ast_free_input,
      (void *)ast_print_input },

    { (void *)ast_parse_list, (void *)ast_eval_list, (void *)ast_free_list,
      (void *)ast_print_list },

    { (void *)ast_parse_if, (void *)ast_eval_if, (void *)ast_free_if,
      (void *)ast_print_if }
};

struct ast_node *ast_create(struct lexer *lexer, enum ast_type type)
{
    struct ast_node *root = calloc(1, sizeof(struct ast_node));
    if (!root)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    root->value = AST_FN[type].parse(lexer);
    return root;
}

int ast_eval(struct ast_node *node, void **out)
{
    return AST_FN[node->type].eval(node->value, out);
}

void ast_free(struct ast_node *node)
{
    AST_FN[node->type].free(node->value);
    free(node);
}

void ast_print(struct ast_node *node)
{
    AST_FN[node->type].print(node->value);
}
