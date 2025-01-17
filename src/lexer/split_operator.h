#ifndef SPLIT_OPERATOR_H
#define SPLIT_OPERATOR_H

#include "mbtstr/str.h"
#include "streams/streams.h"

static const char *OPERATORS[] = { ";", "&&", "&", "|", "||", NULL };
int is_operator(struct mbt_str *str);

int split_operator(struct stream *stream, struct mbt_str *str, char c);
#endif // !SPLIT_OPERATOR_H
