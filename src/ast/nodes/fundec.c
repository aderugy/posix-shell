#include "fundec.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "eval_ctx.h"
#include "node.h"
#include "utils/logger.h"
#include "utils/naming.h"
#include "utils/xalloc.h"

/*
 * fundec = WORD '(' ')' { '\n' } shell_command ;
 */
/*
SCL: case 8 [ NAME in function]
When the TOKEN is exactly a reserved word, the tokenen identifier for that
reserved word shall result. Otherwise, when the TOKEN meets the requirements
for a name, the tokenen identifier NAME shall result. Otherwise, rule 7 applies.
*/
struct ast_fundec *ast_parse_fundec(struct lexer *lexer)
{
    logger("Parse FUNDEC\n");
    struct ast_node *fun = NULL;
    struct ast_fundec *node = xcalloc(1, sizeof(struct ast_fundec));
    node->redirs = list_init();

    // WORD
    struct token *token = lexer_peek(lexer);
    if (!token || !token_is_valid_identifier(token)
        || !XDB_valid(token->value.c) || is_keyword(token->value.c))
    {
        goto error;
    }
    node->name = strdup(token->value.c);
    token_free(lexer_pop(lexer));

    // '('
    if (!(token = lexer_peek(lexer)) || token->type != TOKEN_LEFT_PARENTHESIS)
    {
        goto error;
    }
    token_free(lexer_pop(lexer));

    // ')'
    if (!(token = lexer_peek(lexer)) || token->type != TOKEN_RIGHT_PARENTHESIS)
    {
        lexer_error(lexer, "unmatched parenthesis");
        goto error;
    }
    token_free(lexer_pop(lexer));

    // { \n }
    while ((token = lexer_peek(lexer)) && token->type == TOKEN_NEW_LINE)
    {
        token_free(lexer_pop(lexer));
    }

    // shell_command
    fun = ast_create(lexer, AST_SHELL_COMMAND);
    if (!fun)
    {
        goto error;
    }

    struct ast_node *redir;
    while ((redir = ast_create(lexer, AST_REDIRECTION)))
    {
        list_append(node->redirs, redir);
    }

    logger("Exit FUNDEC (SUCCESS)\n");
    node->fun = fun;
    return node;
error:
    logger("Exit FUNDEC (ERROR)\n");
    if (fun)
    {
        ast_free(fun);
    }

    ast_free_fundec(node);
    return NULL;
}

int ast_eval_fundec(struct ast_fundec *node,
                    __attribute((unused)) struct linked_list *ptr,
                    struct ast_eval_ctx *ctx)
{
    ctx_set_function(ctx, node->name, node->fun);
    node->fun = NULL;
    return AST_EVAL_SUCCESS;
}

void ast_free_fundec(struct ast_fundec *f)
{
    if (f)
    {
        if (f->name)
        {
            free(f->name);
        }

        if (f->fun)
        {
            ast_free(f->fun);
        }

        list_free(f->redirs, (void (*)(void *))ast_free);
        free(f);
    }
}
void ast_print_fundec(struct ast_fundec *f)
{
    logger("%s () {\n", f->name);
    ast_print(f->fun);
    if (f->redirs->size)
    {
        struct linked_list_element *head = f->redirs->head;
        while (head)
        {
            logger(" ");
            ast_print(head->data);
            head = head->next;
        }
    }
    logger("\n}\n");
}
