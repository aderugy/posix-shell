#include "fundec.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/logger.h"
#include "utils/naming.h"

#define TOKEN_GOOD (token && token->type == TOKEN_WORD)

#define FUNDEC_ERROR 2
#define FUNDEC_FREE 4
#define FUNDEC_NOTHING 8

/*
 * fundec = WORD '(' ')' { '\n' } shell_command ;
 */
int sub_parse_fundec(struct lexer *lexer, struct token *token)
{
    int ret_val = FUNDEC_NOTHING;
    if (token->next)
    {
        token = token->next;
        ret_val = FUNDEC_FREE;
    }
    else
    {
        token_free(lexer_pop(lexer));
        token = lexer_peek(lexer);
    }

    if (TOKEN_GOOD && strcmp(token->value.c, "()") == 0)
    {
        if (token->next)
        {
            token = token->next;
            ret_val = FUNDEC_FREE;
        }
        else
        {
            token_free(lexer_pop(lexer));
            token = lexer_peek(lexer);
        }

        /*
        if (!(TOKEN_OK) || strcmp(token->value.c, "{") != 0)
        {
            lexer_error(lexer, "function_def : no {");
            return FUNDEC_ERROR;
        }

        token_free(lexer_pop(lexer));
        */
    }
    else
    {
        lexer_error(lexer, "no '(' was found after WORD\n");
        return FUNDEC_ERROR;
    }
    return ret_val;
}
/*
SCL: case 8 [ NAME in function]
When the TOKEN is exactly a reserved word, the tokenen identifier for that
reserved word shall result. Otherwise, when the TOKEN meets the requirements
for a name, the tokenen identifier NAME shall result. Otherwise, rule 7 applies.
*/
struct ast_fundec *ast_parse_fundec(struct lexer *lexer)
{
    logger("Parse FUNDEC\n");
    struct ast_fundec *node = calloc(1, sizeof(struct ast_fundec));

    // CASE the name of the function
    struct token *token = lexer_peek(lexer);

    if (TOKEN_GOOD && XDB_valid(token->value.c)
        && !(is_keyword(token->value.c)))
    {
        node->name = strdup(token->value.c);

        if (sub_parse_fundec(lexer, token) != FUNDEC_ERROR)
        {
            token = lexer_peek(lexer);

            while (token && token->type == TOKEN_NEW_LINE)
            {
                token_free(lexer_pop(lexer));
                token = lexer_peek(lexer);
            }

            struct ast_node *shell_cmd = ast_create(lexer, AST_SHELL_COMMAND);
            if (!shell_cmd)
            {
                ast_free_fundec(node);
                lexer_error(lexer, "expected shell command");
                return NULL;
            }
            logger("Exit FUNDEC (SUCCESS)\n");
            node->ast_node = shell_cmd;
            return node;
        }
        goto error;
    }
    logger("Exit FUNDEC (EXIT)\n");
error:
    ast_free_fundec(node);
    return NULL;
}
int ast_eval_fundec(struct ast_fundec *f,
                    __attribute__((unused)) struct linked_list *ptr,
                    struct ast_eval_ctx *ctx)
{
    ctx_set_function(ctx, f->name, f->ast_node);
    f->ast_node = NULL;
    return EXIT_SUCCESS;
}
void ast_free_fundec(struct ast_fundec *f)
{
    free(f->name);
    ast_free(f->ast_node);
    free(f);
}
void ast_print_fundec(struct ast_fundec *f)
{
    logger("%s () { ", f->name);
    ast_print(f->ast_node);
    logger(" }");
}
