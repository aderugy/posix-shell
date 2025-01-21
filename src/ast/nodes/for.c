#include "for.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "ast/ast.h"
#include "eval_ctx.h"
#include "lexer/lexer.h"
#include "node.h"
#include "utils/err_utils.h"
#include "utils/logger.h"
#include "utils/xalloc.h"
/*
   rule_for = 'for' WORD ( [';'] | [ {'\n'} 'in' { WORD } ( ';' | '\n' ) ] )
              {'\n'} 'do' compound_list 'done' ;
*/
/* @REFACTOR
 1. clang-tidy function too long
 2. SCL : 'case 5 [ NAME in for]
            When the TOKEN meets the requirements for a name (see XBD Name ),
            the token identifier NAME shall result.
            Otherwise, the token WORD shall be returned.'
 */

static void skip_new_line(struct lexer *lexer)
{
    struct token *tok = NULL;
    while ((tok = lexer_peek(lexer)) && tok->type == TOKEN_NEW_LINE)
    {
        token_free(lexer_pop(lexer));
    }
}

static void check_word_done(struct lexer *lexer)
{
    struct token *tok = NULL;
    if (!(tok = lexer_peek(lexer)) || tok->type != TOKEN_WORD
        || strcmp(tok->value.c, "done"))
    {
        errx(2, "ast_for_node: Syntax error: Bad for loop variable");
        // goto error;
    }
}

static void check_newline_colon(struct lexer *lexer)
{
    struct token *tok = NULL;
    if (!(tok = lexer_peek(lexer))
        || !(tok->type == TOKEN_NEW_LINE || tok->type == TOKEN_SEMICOLON))
    {
        errx(2, "ast_for_node: Syntax error: Bad for loop variable");
        // goto error;
    }
}

static void check_word_in(struct lexer *lexer)
{
    struct token *tok = NULL;
    if (!(tok = lexer_peek(lexer)) || tok->type != TOKEN_WORD
        || strcmp(tok->value.c, "in"))
    {
        errx(2, "ast_for_node: Syntax error: Bad for loop variable");
        //      goto error;
    }
}

static void check_word_do(struct lexer *lexer)
{
    struct token *tok = NULL;
    if (!(tok = lexer_peek(lexer)) || tok->type != TOKEN_WORD
        || strcmp(tok->value.c, "do"))
    {
        errx(2, "ast_for_node: Syntax error: Bad for loop variable");
        // goto error;
    }
}

struct ast_for_node *ast_parse_for(struct lexer *lexer)
{
    logger("PARSE FOR\n");

    struct ast_for_node *ast = xcalloc(1, sizeof(struct ast_for_node));
    CHECK_MEMORY_ERROR(ast);
    ast->items = list_init();

    // "for" keyword
    struct token *tok = lexer_peek(lexer);
    if (!tok || tok->type != TOKEN_WORD || strcmp(tok->value.c, "for") != 0)
    {
        goto error;
    }
    token_free(lexer_pop(lexer));

    // Contains word
    if (!(tok = lexer_peek(lexer)) || tok->type != TOKEN_WORD)
    {
        errx(2, "ast_for_node: Syntax error: Bad for loop variable");
        // goto error;
    }
    ast->name = strdup(tok->value.c);
    token_free(lexer_pop(lexer));

    // [ ; ]
    if ((tok = lexer_peek(lexer)) && tok->type == TOKEN_SEMICOLON)
    {
        token_free(lexer_pop(lexer));
        list_append(ast->items, strdup("$@"));
    }
    else
    {
        // { \n }
        skip_new_line(lexer);

        // 'in'
        check_word_in(lexer);

        token_free(lexer_pop(lexer));

        // { WORD }
        while ((tok = lexer_peek(lexer)) && tok->type == TOKEN_WORD)
        {
            list_append(ast->items, strdup(tok->value.c));
            token_free(lexer_pop(lexer));
        }

        check_newline_colon(lexer);

        token_free(lexer_pop(lexer));
    }

    // { '\n' }
    skip_new_line(lexer);

    // 'do'
    check_word_do(lexer);
    token_free(lexer_pop(lexer));

    if (!(ast->body = ast_create(lexer, AST_CLIST)))
    {
        errx(2, "ast_for_node: Syntax error: Bad for loop variable");
        // goto error;
    }

    check_word_done(lexer);
    token_free(lexer_pop(lexer));

    logger("EXIT FOR (SUCCESS)\n");
    return ast;

error:
    ast_free_for(ast);
    logger("EXIT FOR (ERROR)\n");
    return NULL;
}

int ast_eval_for(struct ast_for_node *node, __attribute((unused)) void **out,
                 struct ast_eval_ctx *ctx)
{
    /*
     * Note: to handle for loops properly, we need to work on field splitting
     * ie: for each character in IFS, the expansion of a parameter, arithmetic
     * expression, subshell or globbing must be separated by the character
     */

    int ret_val = AST_EVAL_SUCCESS;
    struct linked_list_element *item = node->items->head;
    while (item)
    {
        ast_eval_ctx_set_local_var(ctx, node->name, item->data);
        ret_val = ast_eval(node->body, NULL, ctx);
        item = item->next;
        if (ctx->break_count > 0)
        {
            ctx->break_count--;
            return ret_val;
        }
        if (ctx->continue_count > 1)
        {
            ctx->continue_count--;
            return ret_val;
        }
        else if (ctx->continue_count == 1)
        {
            ctx->continue_count--;
        }
    }

    return ret_val;
}

void ast_free_for(struct ast_for_node *node)
{
    if (node)
    {
        if (node->name)
        {
            free(node->name);
        }

        if (node->items)
        {
            list_free(node->items, free);
        }

        if (node->body)
        {
            ast_free(node->body);
        }

        free(node);
    }
}

void ast_print_for(struct ast_for_node *node)
{
    logger("for %s in", node->name);
    for (struct linked_list_element *elt = node->items->head; elt;
         elt = elt->next)
    {
        logger(" %s", elt->data);
    }
    logger("; do ");
    ast_print(node->body);
    logger("; done");
}
