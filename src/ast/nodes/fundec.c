#include "fundec.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "utils/logger.h"
#include "utils/naming.h"

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
    struct ast_fundec *node = calloc(1, sizeof(struct ast_fundec));

    // CASE the name of the function
    struct tokenen *token = lexer_peek(lexer);
    if (TOKEN_OK && XDB_valid(token->value.c) && !(is_keyword(token->value.c)))
    {
        node->name = strdup(token->value.c);
        lexer_pop(lexer);
        tokenen_free(token);

        token = lexer_peek(lexer);
        if (reserved_word_check(token) && strcmp(token->value.c, "(") == 0)
        {
            lexer_pop(lexer);
            tokenen_free(token);
            token = lexer_peek(lexer);

            if (reserved_word_check(token) && strcmp(token->value.c, ")") == 0)
            {
                lexer_pop(lexer);
                tokenen_free(token);
                token = lexer_peek(lexer);
                while (token && token->type == TOKEN_NEW_LINE)
                {
                    lexer_pop(lexer);
                    tokenen_free(token);
                    token = lexer_peek(lexer);
                }

                struct ast_node *shell_cmd =
                    ast_create(lexer, AST_SHELL_COMMAND);
                if (!shell_cmd)
                {
                    ast_free_fundec(node);
                    logger("no shell command was found after () { \\n }\n");
                    logger("Exit FUNDEC (FAILED)\n");
                    return NULL;
                }
                logger("Exit FUNDEC (SUCCESS)\n");
                node->ast_node = shell_cmd;
                return node;
            }
        }

        ast_free_fundec(node);
        logger("no '(' was found after WORD\n");
        logger("Exit FUNDEC (FAILED)\n");
        return NULL;
    }
    logger("Exit FUNDEC (EXIT)\n");
    ast_free_fundec(node);
    return NULL;
}
int ast_eval_fundec(struct ast_fundec *f, void **ptr, struct ast_eval_ctx *ctx)
{
    return ast_eval(f->ast_node, ptr, ctx);
}
void ast_free_fundec(struct ast_fundec *f)
{
    free(f->name);
    ast_free(f->ast_node);
    free(f);
}
void ast_print_fundec(struct ast_fundec *f)
{
    printf("function name : %s\n", f->name);
    ast_print(f->ast_node);
}
