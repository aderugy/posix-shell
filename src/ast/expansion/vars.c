#include "vars.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer/splitter.h"
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
