#ifndef REDIRECTION_DEFINITION_H
#define REDIRECTION_DEFINITION_H
#include "utils/eval_output_structure.h"
#include "utils/linked_list.h"

void save_fd(int fd, int fd2, int saved_stdout, struct linked_list *out);
#endif // !REDIRECTION_DEFINITION_H
