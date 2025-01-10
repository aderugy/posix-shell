#include <err.h>
#include <fcntl.h>
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
    size_t i;
    int status;
    int pipefds[2];
    int in_fd = STDIN_FILENO;
    int stdout_fd = dup(STDOUT_FILENO);
    size_t n = linked_list->size;
    for (size_t i = 0; i < n; i++)
    {
        if (i < n - 1)
        {
            int p =
                pipe(pipefds); // Créer un pipe sauf pour la dernière commande
            if (p == -1)
                errx(2, "exec_pipeline: error pipe");
        }

        pid_t pid = fork();
        if (pid == 0)
        { // CHILD
            if (in_fd != STDIN_FILENO)
            {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (i < n - 1)
            {
                dup2(pipefds[1], STDOUT_FILENO);
                close(pipefds[1]);
            }
            else
            {
                dup2(stdout_fd, STDOUT_FILENO);
                if (pipefds[0] != -1)
                    close(pipefds[0]);
                if (pipefds[1] != -1)
                    close(pipefds[1]);
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

        // Parent : Fermer descripteurs inutilisés
        if (in_fd != STDIN_FILENO)
            close(in_fd);
        if (i < n - 1)
            close(pipefds[1]);

        in_fd = pipefds[0]; // Préparer l'entrée pour la prochaine commande
    }
    close(stdout_fd);
    for (i = 0; i < n; i++)
    {
        wait(&status);
    }

    return WEXITSTATUS(status);
}
