#ifndef EVAL_OUTPUT_STRUCTURE_H
#define EVAL_OUTPUT_STRUCTURE_H

enum eval_output_type
{
    EVAL_STR,
    EVAL_FD
};

struct eval_output
{
    union
    {
        char *str;
        int fd;
    } value;
    enum eval_output_type type;
};

struct eval_output *eval_output_init(enum eval_output_type type);
void eval_output_free(struct eval_output *eval_output);
#endif // !EVAL_OUTPUT_STRUCTURE_H
