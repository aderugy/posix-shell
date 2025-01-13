#include "element.h"

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "lexer/lexer.h"
#include "lexer/token.h"
#include "mbtstr/str.h"
#include "node.h"
#include "utils/logger.h"

/**
 * element = WORD
 * | redirection ;
 */

static const struct keywords KEYWORDS[] = {
    { "if", TOKEN_IF },       { "fi", TOKEN_FI },     { "elif", TOKEN_ELIF },
    { "else", TOKEN_ELSE },   { "then", TOKEN_THEN }, { "while", TOKEN_WHILE },
    { "until", TOKEN_UNTIL }, { "for", TOKEN_FOR },   { "do", TOKEN_DO },
    { "done", TOKEN_DONE }
};

#define KEYWORD_LEN (sizeof(KEYWORDS) / sizeof(KEYWORDS[0]))

char *get_word_token(struct token *token)
{
    char *mot = NULL;
    size_t i = 0;
    for (; i < KEYWORD_LEN; i++)
    {
        if (token->type == KEYWORDS[i].type)
        {
            mot = KEYWORDS[i].name;
            break;
        }
    }
    if (mot == NULL)
    {
        return NULL;
    }
    size_t len = strlen(mot);
    char *result = calloc(1, len + 1);
    for (size_t i = 0; i < len; i++)
    {
        result[i] = mot[i];
    }
    return result;
}

struct ast_element *ast_parse_element(struct lexer *lexer)
{
    logger("Parse ELEMENT\n");
    struct token *token = lexer_peek(lexer); // We check if the token is valid
    if (!token)
    {
        logger("Exit ELEMENT\n");
        return NULL;
    }

    struct ast_element *node = calloc(1, sizeof(struct ast_element));
    if (!node)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    if (token->type == TOKEN_WORD)
    {
        lexer_pop(lexer); // Valid token -> we consume it
        node->value = token->value.c;
        free(token);
        logger("Exit ELEMENT\n");
        return node;
    }
    char *word = get_word_token(token);
    if (word != NULL)
    {
        lexer_pop(lexer);
        node->value = word;
        free(token);
        return node;
    }

    struct ast_node *redir = ast_create(lexer, AST_REDIRECTION);
    if (!redir)
    {
        ast_free_element(node);
        logger("Exit ELEMENT\n");
        return NULL;
    }

    node->redir = redir;
    logger("Exit ELEMENT\n");
    return node;
}

int ast_eval_element(struct ast_element *node, void **out)
{
    *out = node->value;

    if (node->redir)
    {
        ast_eval(node->redir, NULL);
    }

    return AST_EVAL_SUCCESS;
}

void ast_free_element(struct ast_element *node)
{
    if (node->value)
    {
        free(node->value);
    }
    if (node->redir)
    {
        ast_free(node->redir);
    }

    free(node);
}

void ast_print_element(__attribute((unused)) struct ast_element *node)
{
    if (node->value)
    {
        logger("%s", node->value);
    }
    if (node->redir)
    {
        ast_print(node->redir);
    }
}
