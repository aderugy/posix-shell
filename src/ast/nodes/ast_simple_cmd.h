#ifndef AST_SIMPLE_CMD_H
#define AST_SIMPLE_CMD_H

struct ast_simple_cmd
{
    struct ast_node *prefix;
    struct linked_list *prefixes;

    char *cmd;
    struct linked_list *args;
};

#endif // !AST_SIMPLE_CMD_H
