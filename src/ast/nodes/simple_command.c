#include "simple_command.h"

#include <err.h>
#include <stdlib.h>

#include "lexer/token.h"
#include "node.h"
#include "utils/linked_list.h"

struct ast_simple_cmd *ast_parse_simple_cmd(struct lexer *lexer)
{
    struct token *token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        return NULL;
    }

    struct ast_simple_cmd *cmd = calloc(1, sizeof(struct ast_simple_cmd));
    if (!cmd)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    struct linked_list *list = list_init();
    while ((token = lexer_peek(lexer)))
    {
        list_append(list, token);
        token = lexer_pop(lexer);
    }

    cmd->args = list;
    return cmd;
}

int ast_eval_simple_cmd(struct ast_simple_cmd *cmd, void **out);

void ast_free_simple_cmd(struct ast_simple_cmd *cmd)
{
    list_free(cmd->args, ast_free);
    free(cmd);
}
