#ifndef AST_REDIRECTION
#define AST_REDIRECTION

#include "element.h"
#include "ionumber.h"
#include "lexer/lexer.h"
#include "node.h"

struct ast_redir
{
    struct ast_node *number;
    char *pipe;
    struct ast_node *word;
};

struct ast_redir *ast_parse_redir(struct lexer *lexer);
int ast_eval_redir(struct ast_redir *node);
void ast_free_redir(struct ast_redir *node);
void ast_print_redir(struct ast_redir *node);

#endif // !ast_redirection
