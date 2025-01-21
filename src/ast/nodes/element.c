#include "element.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "mbtstr/str.h"
#include "node.h"
#include "utils/logger.h"
#include "utils/xalloc.h"

/**
 * element = WORD
 * | redirection ;
 */

struct ast_element *ast_parse_element(struct lexer *lexer)
{
    struct ast_element *node = xcalloc(1, sizeof(struct ast_element));

    struct ast_node *cword = ast_create(lexer, AST_COMPLEX_WORD);
    if (cword)
    {
        node->type = AST_ELEMENT_WORD;
        node->child = cword;
        return node;
    }

    struct ast_node *redir = ast_create(lexer, AST_REDIRECTION);
    if (redir)
    {
        node->type = AST_ELEMENT_REDIR;
        node->child = redir;
        return node;
    }

    ast_free_element(node);
    return NULL;
}

int ast_eval_element(struct ast_element *node, struct linked_list *out,
                     struct ast_eval_ctx *ctx)
{
    if (node->type == AST_ELEMENT_WORD && !ctx->check_redir)
    {
        return ast_eval(node->child, out, ctx);
    }
    else if (node->type == AST_ELEMENT_REDIR && ctx->check_redir)
    {
        int redir_eval_result = ast_eval(node->child, out, ctx);
        if (redir_eval_result != 0)
        {
            return redir_eval_result;
        }

        return -1;
    }

    return 0;
}

void ast_free_element(struct ast_element *node)
{
    if (node)
    {
        if (node->child)
        {
            ast_free(node->child);
        }

        free(node);
    }
}

void ast_print_element(struct ast_element *node)
{
    if (node->child)
    {
        ast_print(node->child);
    }
}
