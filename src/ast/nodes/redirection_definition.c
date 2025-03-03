#include "redirection_definition.h"
void save_fd(int fd, int fd2, int saved_stdout, struct linked_list *out)
{
    if (out)
    {
        struct eval_output *eval_output_fd_1 = eval_output_init(EVAL_FD);
        struct eval_output *eval_output_fd_2 = eval_output_init(EVAL_FD);
        struct eval_output *eval_output_fd_3 = eval_output_init(EVAL_FD);

        eval_output_fd_1->value.fd = fd;
        eval_output_fd_2->value.fd = fd2;
        eval_output_fd_3->value.fd = saved_stdout;

        list_append(out, eval_output_fd_1);
        list_append(out, eval_output_fd_2);
        list_append(out, eval_output_fd_3);
    }
}
