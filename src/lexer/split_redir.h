#ifndef SPLIT_REDIR_H
#define SPLIT_REDIR_H
#include <stddef.h>

#include "mbtstr/str.h"
#include "streams/streams.h"

static const char *REDIRS[] = { ">>", ">&", "<&", "<>", ">", "<", NULL };
int is_redir(struct mbt_str *str);

int split_redir(struct stream *stream, struct mbt_str *str, char c);
#endif // !SPLIT_REDIR_H
