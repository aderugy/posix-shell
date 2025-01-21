#include "vars.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer/splitter.h"
#include "utils/my_strcat.h"

// $#
void init_hashtag(int nb_arg, struct ast_eval_ctx *ctx)
{
    char *nb_arg_str = calloc(1, 64);
    hash_map_insert(ctx->value, "#", my_itoa(nb_arg, nb_arg_str));
    free(nb_arg_str);
}

// $$
void init_dollar(struct ast_eval_ctx *ctx)
{
    pid_t pid = getpid();
    char *dollar_arg = calloc(1, 64);
    hash_map_insert(ctx->value, "$", my_itoa(pid, dollar_arg));
    free(dollar_arg);
}

// $?
void update_qm(struct ast_eval_ctx *ctx, int ret_val)
{
    char *qm_arg = calloc(1, 65);
    hash_map_insert(ctx->value, "?", my_itoa(ret_val, qm_arg));
    free(qm_arg);
}

// inits the number of args
int init_args(int argc, char *argv[], struct ast_eval_ctx *ctx)
{
    char *arobase = calloc(1, 64);
    for (int i = 2; i < argc; i++)
    {
        //  FOR $@
        arobase = my_strcat(arobase, argv[i]);
        if (i < argc - 1)
        {
            arobase = my_strcat(arobase, " ");
        }

        char *number = calloc(1, 65);
        hash_map_insert(ctx->value, my_itoa(i - 1, number), argv[i]);
        free(number);
    }

    hash_map_insert(ctx->value, "@", arobase);

    // Temporary for $*
    hash_map_insert(ctx->value, "*", arobase);
    free(arobase);

    return argc - 2;
}

//$UID
void init_UID(struct ast_eval_ctx *ctx)
{
    uid_t uid = getuid();
    char *uid_arg = calloc(1, 65);
    hash_map_insert(ctx->value, "UID", my_itoa(uid, uid_arg));
    free(uid_arg);
}
