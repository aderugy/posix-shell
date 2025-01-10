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
        /*if (i < n - 1)
        {
            int p =
                pipe(pipefds); // Créer un pipe sauf pour la dernière commande
            if (p == -1)
                errx(2, "exec_pipeline: error pipe");
        }*/

        pid_t pid = fork();
        if (pid == 0)
        { // CHILD

            if (j != 0)
            {
                if (dup2(pipefds[j - 2], 0) < 0)
                {
                    perror(" dup2");
                    exit(EXIT_FAILURE);
                }
            }
            if (i < n - 1)
            {
                if (dup2(pipefds[j + 1], 1) < 0)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
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
            perror("fork");
            return 1;
        }

        j += 2;
    }
    for (size_t k = 0; k < 2 * n; k++)
    {
        close(pipefds[k]);
    }
    for (size_t k = 0; k < 2 * n; k++)
    {
        wait(&status);
    }
    free(pipefds);

    return WEXITSTATUS(status);
}
