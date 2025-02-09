#ifndef HISTORY_H
#define HISTORY_H

#include <stdlib.h>
#include <string.h>

#define HISTORY_SIZE 20

extern char *history[HISTORY_SIZE];
extern int history_count;
extern int history_pos;

void add_to_history(char *command);

#endif // HISTORY_H