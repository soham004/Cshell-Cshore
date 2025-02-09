#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>

#define buffer_size 1024
#define token_buf_size 64
#define token_delim " \t\n\r\a"

char *shell_read_line(void);
char **shell_split(char *line);

#endif // INPUT_H