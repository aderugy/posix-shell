#include "vars.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer/splitter.h"

// $#
void init_hashtag(int nb_arg, struct ast_eval_ctx *ctx)
{
    char *nb_arg_str = calloc(1, 64);
    char *key = calloc(1, 2);
    key[0] = '#';
    struct mbt_str *str_dieze = mbt_str_init(8);
    mbt_str_pushcstr(str_dieze, my_itoa(nb_arg, nb_arg_str));
    free(nb_arg_str);
    hash_map_insert(ctx->value, key, str_dieze);
}

// $$
void init_dollar(struct ast_eval_ctx *ctx)
{
    pid_t pid = getpid();
    char *key_dollar = calloc(1, 2);
    key_dollar[0] = '$';
    char *dollar_arg = calloc(1, 65);
    struct mbt_str *str_dollar = mbt_str_init(8);
    mbt_str_pushcstr(str_dollar, my_itoa(pid, dollar_arg));
    free(dollar_arg);
    hash_map_insert(ctx->value, key_dollar, str_dollar);
}

// $?
void update_qm(struct ast_eval_ctx *ctx, int ret_val)
{
    char *key_qm = calloc(1, 2);
    key_qm[0] = '?';

    char *qm_arg = calloc(1, 65);
    struct mbt_str *str_qm = mbt_str_init(8);
    mbt_str_pushcstr(str_qm, my_itoa(ret_val, qm_arg));

    free(qm_arg);

    hash_map_insert(ctx->value, key_qm, str_qm);
}

// inits the number of args
int init_args(int argc, char *argv[], struct ast_eval_ctx *ctx)
{
    struct mbt_str *arobase = mbt_str_init(64);

    for (int i = 2; i < argc; i++)
    {
        struct mbt_str *str = mbt_str_init(8);
        mbt_str_pushcstr(str, argv[i]);

        //  FOR $@
        mbt_str_pushcstr(arobase, str->data);
        if (i < argc - 1)
        {
            mbt_str_pushc(arobase, ' ');
        }

        char *number = calloc(1, 65);
        hash_map_insert(ctx->value, my_itoa(i - 1, number), str);
    }

    char *karabose = calloc(1, 2);
    karabose[0] = '@';

    hash_map_insert(ctx->value, karabose, arobase);

    // Temporary for $*
    char *glob = calloc(1, 2);
    glob[0] = '*';
    struct mbt_str *glob_val = mbt_str_init(8);
    mbt_str_pushcstr(glob_val, arobase->data);

    hash_map_insert(ctx->value, glob, glob_val);

    return 2 - argc;
}

//$UID
void init_UID(struct ast_eval_ctx *ctx)
{
    uid_t uid = getuid();
    char *key_uid = strdup("UID");
    char *uid_arg = calloc(1, 65);
    struct mbt_str *str_uid = mbt_str_init(8);
    mbt_str_pushcstr(str_uid, my_itoa(uid, uid_arg));
    free(uid_arg);
    hash_map_insert(ctx->value, key_uid, str_uid);
}
