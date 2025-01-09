#ifndef COMMANDS_H
#define COMMANDS_H

#define ECHO "echo"
#define TRUE_BUILTIN "true_builtin"
#define FALSE_BUILTIN "false_builtin"

int register_commands(void);
int unregister_commands(void);

int echo(int argc, char *argv[]);
int true_builtin(int argc, char **argv);
int false_builtin(int argc, char **argv);

#endif // !COMMANDS_H
