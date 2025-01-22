#include "vars.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer/splitter.h"
#include "utils/linked_list.h"
#include "utils/xalloc.h"

// $#
void ctx_init_local_hashtag(int nb_arg, struct ast_eval_ctx *ctx)
{
    char *buffer = xcalloc(15, sizeof(char));
    char *str = my_itoa(nb_arg, buffer);

    ctx_set_local_variable(ctx, "#", str);

    free(str);
}

// $$
void ctx_init_local_dollar(struct ast_eval_ctx *ctx)
{
    pid_t pid = getpid();

    char *buffer = xcalloc(15, sizeof(char));
    char *str = my_itoa(pid, buffer);

    ctx_set_local_variable(ctx, "$", str);

    free(str);
}

// $?
void ctx_update_local_qm(struct ast_eval_ctx *ctx, int return_value)
{
    char *buffer = xcalloc(15, sizeof(char));
    char *str = my_itoa(return_value, buffer);

    ctx_set_local_variable(ctx, "?", str);

    free(str);
}

//$UID
void ctx_init_local_UID(struct ast_eval_ctx *ctx)
{
    uid_t uid = getuid();

    char *buffer = xcalloc(15, sizeof(char));
    char *str = my_itoa(uid, buffer);

    char *name = strdup("UID");

    ctx_set_local_variable(ctx, name, str);

    free(str);
    free(name);
}
// inits $1, $2, $3, $n + the arobase and the star
// @return the numbers of $ args
int ctx_init_local_args(int argc, char *argv[], struct ast_eval_ctx *ctx)
{
    // FOR $@ = $1 + IFS + $2 + IFS + etc
    struct mbt_str *arobase_args = mbt_str_init(64);

    // $0
    ctx_set_local_variable(ctx, "0", argv[0]);

    for (int i = 2; i < argc; i++)
    {
        // Accumlution to form the list of arguments
        mbt_str_pushcstr(arobase_args, argv[i]);

        if (i < argc - 1)
        {
            // $@ += IFS
            mbt_str_pushc(arobase_args, ' ');
        }

        char *buffer = xcalloc(12, sizeof(char));
        char *name = my_itoa(i - 1, buffer);
        char *str = argv[i];

        ctx_set_local_variable(ctx, name, str);
        free(name);
    }

    char *str = arobase_args->data;

    ctx_set_local_variable(ctx, "@", str);

    // Until I understand it better, the arobase is the same as the star
    ctx_set_local_variable(ctx, "*", str);

    mbt_str_free(arobase_args);

    return argc - 2;
}

// classic variables are global but function parameters are local
// saves the parameters value before entering a function
// so we save them to restore them right after the function call
struct linked_list *ctx_save_spe_vars(struct ast_eval_ctx *ctx)
{
    struct linked_list *list = list_init();

    list_append(list, strdup(ctx_get_variable(ctx, "@")));
    list_append(list, strdup(ctx_get_variable(ctx, "*")));
    list_append(list, strdup(ctx_get_variable(ctx, "?")));
    list_append(list, strdup(ctx_get_variable(ctx, "$")));
    list_append(list, strdup(ctx_get_variable(ctx, "#")));

    int nb_args = atoi(ctx_get_variable(ctx, "#"));

    // handles $1, $2, $3, etc
    for (int i = 1; i <= nb_args; i++)
    {
        char *buffer = xcalloc(12, sizeof(char));
        char *name = my_itoa(i, buffer);

        list_append(list, strdup(ctx_get_variable(ctx, name)));
        free(name);
    }

    return list;
}
// Restore a previous parameter context
// use right after a function call to restore the parameters
void ctx_restore_spe_vars(struct ast_eval_ctx *ctx, struct linked_list *old_ctx)
{
    ctx_set_local_variable(ctx, "@", list_get(old_ctx, 0));
    ctx_set_local_variable(ctx, "*", list_get(old_ctx, 1));
    ctx_set_local_variable(ctx, "?", list_get(old_ctx, 2));
    ctx_set_local_variable(ctx, "$", list_get(old_ctx, 3));
    ctx_set_local_variable(ctx, "#", list_get(old_ctx, 4));

    for (unsigned int i = 5; i < old_ctx->size; i++)
    {
        char *buffer = xcalloc(12, sizeof(char));
        // shift to get i at the correct value eg: $1, $2, etc
        char *name = my_itoa(i - 4, buffer);

        ctx_set_local_variable(ctx, name, list_get(old_ctx, i));

        free(name);
    }
}
