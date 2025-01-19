#ifndef COMMANDS_H
#define COMMANDS_H

#define ECHO "echo"
#define TRUE "true"
#define FALSE "false"
#define EXIT "exit"
#define CD "cd"
#define EXPORT "export"
#define UNSET "unset"

int register_commands(void);
int unregister_commands(void);

int echo(int argc, char *argv[]);
int true_builtin(int argc, char **argv);
int false_builtin(int argc, char **argv);
int exit_builtin(int argc, char **argv);
int cd(int argc, char **argv);
int export_builtin(int argc, char *argv[]);
int unset_builtin(int argc, char *argv[]);

#endif // !COMMANDS_H
