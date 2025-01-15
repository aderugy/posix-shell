#include <err.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ast/nodes/node.h"
#include "linked_list.h"

int exec_pipeline(struct linked_list *linked_list)
{
    int status;
    size_t n = linked_list->size;
    int *pipefds = calloc(2 * n, sizeof(int));
    pid_t *pid_tab = calloc(n, sizeof(pid_t));
    for (size_t l = 0; l < n; l++)
    {
        if (pipe(pipefds + l * 2) < 0)
        {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }
    int j = 0;
    for (size_t i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        { // CHILD

            if (j != 0)
            {
                if (dup2(pipefds[j - 2], 0) < 0)
                {
                    errx(1, "pipeline: dup2 error");
                }
            }
            if (i < n - 1)
            {
                if (dup2(pipefds[j + 1], 1) < 0)
                {
                    errx(1, "pipeline: dup2 error");
                }
            }

            for (size_t k = 0; k < 2 * n; k++)
            {
                close(pipefds[k]);
            }
            struct ast_node *ast_node = list_get(linked_list, i);
            int result = ast_eval(ast_node, NULL);
            _exit(result);
        }
        else if (pid < 0)
        {
            errx(1, "pipeline: pid error");
        }
        pid_tab[i] = pid;

        j += 2;
    }
    for (size_t k = 0; k < 2 * n; k++)
    {
        close(pipefds[k]);
    }
    for (size_t k = 0; k < n; k++)
    {
        waitpid(pid_tab[k], &status, 0);
    }
    free(pipefds);
    free(pid_tab);

    return WEXITSTATUS(status);
}
