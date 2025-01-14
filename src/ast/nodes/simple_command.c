#include "simple_command.h"

#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/ast.h"
#include "builtins/builtins.h"
#include "builtins/commands.h"
#include "builtins/run_command.h"
#include "lexer/token.h"
#include "node.h"
#include "utils/linked_list.h"
#include "utils/logger.h"

static char *keywords[] = { "then", "elif", "if",    "fi",    "else", "do",
                            "for",  "done", "while", "until", NULL };

bool is_keyword(char *word)
{
    for (size_t i = 0; keywords[i]; i++)
    {
        if (strcmp(keywords[i], word) == 0)
        {
            logger("\tsimple command : found keyword %s\n", word);
            return true;
        }
    }
    return false;
}

struct ast_simple_cmd *ast_parse_simple_cmd(struct lexer *lexer)
{
    logger("\tParse SIMPLE_COMMAND\n");
    struct ast_simple_cmd *cmd = calloc(1, sizeof(struct ast_simple_cmd));
    if (!cmd)
    {
        errx(EXIT_FAILURE, "out of memory");
    }

    struct token *token = NULL;
    cmd->prefix = ast_create(lexer, AST_PREFIX);
    cmd->prefixes = list_init();
    cmd->cmd = NULL;
    cmd->args = list_init();

    struct ast_node *prefix;
    while ((prefix = ast_create(lexer, AST_PREFIX)))
    {
        list_append(cmd->prefixes, prefix);
    }

    if (cmd->prefix)
    {
        // prefix { prefix }
        logger("\tExit SIMPLE_COMMAND: RULE 1\n");
        return cmd;
    }

    // { prefix } WORD { element }
    token = lexer_peek(lexer);
    if (token->type != TOKEN_WORD)
    {
        goto error;
    }
    if (token->value.c && is_keyword(token->value.c))
    {
        ast_free_simple_cmd(cmd);
        return NULL;
    }
    logger("\t SIMPLE_COMMAND : found cmd : %s\n", token->value.c);

    cmd->cmd = token->value.c;
    free(lexer_pop(lexer));

    struct ast_node *element;
    while ((element = ast_create(lexer, AST_ELEMENT)))
    {
        list_append(cmd->args, element);
    }

    logger("Exit SIMPLE_COMMAND: RULE 2\n");
    return cmd;
error:
    ast_free_simple_cmd(cmd);
    logger("Exit SIMPLE_COMMAND\n");
    return NULL;
}

int ast_eval_simple_cmd(struct ast_simple_cmd *cmd,
                        __attribute((unused)) void **out)
{
    if (!cmd->cmd)
    {
        logger("Eval SIMPLE_COMMAND: RULE 1\n");
        return ast_eval(cmd->prefix, NULL);
    }
    logger("Eval SIMPLE_COMMAND: RULE 2\n");
    size_t argc = cmd->args->size + 1;
    char **argv = calloc(argc, sizeof(char *));
    argv[0] = cmd->cmd;
    size_t elt = 1;

    logger("Nombre d\'arguments: %lu\n", elt);
    logger("simple command : execute : %s\n", argv[0]);
    int ret_value = run_command(elt, argv);
    int stat;
    if (ret_value == 127)
    {
        logger("simple command : not a builtin\n");

        pid_t p = fork();
        if (p == 0)
        {
            for (size_t i = 1; i < argc; i++)
            {
                struct ast_node *children = list_get(cmd->args, i - 1);

                if (ast_eval(children, (void **)argv + elt) == 0)
                    elt++;
                logger("Nombre d\'argument: %lu\n", elt);
            }
            logger("sortie de boucle\n");
            ret_value = execvp(argv[0], argv);
            exit(ret_value);
        }
        else
        {
            wait(&stat);
            int result = WEXITSTATUS(stat);

            if (result == 255)
            {
                errx(127, "simple_command: command not found");
            }
            free(argv);
            return result;
        }
    }

    free(argv);
    return ret_value;
}

void ast_free_simple_cmd(struct ast_simple_cmd *cmd)
{
    if (cmd->prefix)
    {
        ast_free(cmd->prefix);
    }
    list_free(cmd->prefixes, (void (*)(void *))ast_free);

    if (cmd)
    {
        free(cmd->cmd);
    }
    list_free(cmd->args, (void (*)(void *))ast_free);

    free(cmd);
}

void ast_print_simple_cmd(struct ast_simple_cmd *cmd)
{
    logger("command");

    struct linked_list_element *head = cmd->args->head;
    while (head)
    {
        struct ast_node *node = head->data;
        logger(" ");
        ast_print(node);
        head = head->next;
    }
}
