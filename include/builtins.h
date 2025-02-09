#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern int DEBUG;

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
int shell_execute(char **args);

#endif // BUILTINS_H