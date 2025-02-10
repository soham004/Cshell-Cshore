#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>

extern int DEBUG;

void sigint_handler(int signo);
void enableRawMode();
void disableRawMode();
int shell_launch(char **args);
char *autocomplete(const char *prefix);

#endif // UTILS_H