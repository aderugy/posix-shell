#ifndef EVAL_OUTPUT_STRUCTURE_H
#define EVAL_OUTPUT_STRUCTURE_H

struct eval_output
{
    union
    {
        char *str;
        int fd;
    } value;
};

struct eval_output *eval_output_init(void);
void eval_output_free(struct eval_output *eval_output);
#endif // !EVAL_OUTPUT_STRUCTURE_H
