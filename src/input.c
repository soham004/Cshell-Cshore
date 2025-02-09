#include "../include/input.h"
#include "../include/history.h"
#include "../include/utils.h"
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/limits.h>

char *shell_read_line(void) {
    enableRawMode();

    int bufferSize = buffer_size, pos = 0;
    char *buffer = malloc(bufferSize);
    char cwd[PATH_MAX];
    int c;
    int cursor_pos = 0;

    if (!buffer) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();

        if (c == 27) { // Escape sequence for arrow keys
            getchar(); // Skip '['
            switch (getchar()) {
                case 'A': // Up Arrow
                    if (history_count > 0 && history_pos > 0) {
                        history_pos--;
                        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                            printf("\r\033[K%s> ", cwd); // Clear line
                        }
                        printf("%s", history[history_pos]); 
                        strcpy(buffer, history[history_pos]);
                        pos = strlen(buffer);
                        cursor_pos = pos;
                    }
                    continue;
                case 'B': // Down Arrow
                    if (history_count > 0 && history_pos < history_count - 1) {
                        history_pos++;
                        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                            printf("\r\033[K%s> ", cwd);
                        }
                        printf("%s", history[history_pos]); // Print next history entry
                        strcpy(buffer, history[history_pos]);
                        pos = strlen(buffer);
                        cursor_pos = pos;
                    }
                    continue;
                case 'C': // Right Arrow
                    if (cursor_pos < pos) {
                        printf("\033[C"); // Move cursor right
                        cursor_pos++;
                    }
                    continue;
                case 'D': // Left Arrow
                    if (cursor_pos > 0) {
                        printf("\033[D"); // Move cursor left
                        cursor_pos--;
                    }
                    continue;
            }
        }

        if (c == '\n' || c == EOF) {
            printf("\n");
            buffer[pos] = '\0';
            break;
        } 
        else if (c == 127) { // Handle Backspace
            if (pos > 0 && cursor_pos > 0) {
                printf("\b \b"); // Erase character
                memmove(&buffer[cursor_pos - 1], &buffer[cursor_pos], pos - cursor_pos);
                pos--;
                cursor_pos--;
                buffer[pos] = '\0';
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    printf("\r\033[K%s> %s", cwd, buffer); // Clear line and print buffer
                } // Clear line and print buffer
                for (int i = pos; i > cursor_pos; i--) {
                    printf("\033[D"); // Move cursor back to the correct position
                }
            }
        } 
        else if (c == '\t') { // Handle Tab for auto-completion
            buffer[pos] = '\0';
            char *completion = autocomplete(buffer);
            if (completion) {
                strcpy(buffer, completion);
                pos = strlen(buffer);
                cursor_pos = pos;
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    printf("\r\033[K%s> %s", cwd, buffer); // Clear line and print buffer
                }
                free(completion);
            }
        }
        else {
            memmove(&buffer[cursor_pos + 1], &buffer[cursor_pos], pos - cursor_pos);
            buffer[cursor_pos] = c;
            pos++;
            cursor_pos++;
            buffer[pos] = '\0';
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("\r\033[K%s> %s", cwd, buffer); // Clear line and print buffer
            }
            for (int i = pos; i > cursor_pos; i--) {
                printf("\033[D"); // Move cursor back to the correct position
            }
        }

        if (pos >= bufferSize) {
            bufferSize += buffer_size;
            buffer = realloc(buffer, bufferSize);
            if (!buffer) {
                fprintf(stderr, "shell: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    disableRawMode();
    return buffer;
}

char **shell_split(char *line) {
    int buf_size = token_buf_size, position = 0;
    char **tokens = malloc(sizeof(char*) * buf_size);
    char *ptr = line, *token_start;
    int in_quotes = 0;

    if (!tokens) {
        fprintf(stderr, "shell: token allocation error\n");
        exit(EXIT_FAILURE);
    }

    printf(DEBUG ? "Args:\n" : "");

    while (*ptr) {
        // Skip leading delimiters
        while (*ptr && strchr(token_delim, *ptr)) ptr++;

        if (*ptr == '\0') break; // End of input

        // Check if token starts with a quote
        if (*ptr == '"') {
            in_quotes = 1;
            token_start = ++ptr; // Move past opening quote
            while (*ptr && (*ptr != '"' || (*(ptr - 1) == '\\'))) {
                ptr++; // Move inside the quotes
            }
            *ptr = '\0'; // Null terminate the token at the closing quote
            ptr++; // Move past the closing quote
        } 
        else {
            // Regular token (space-separated)
            token_start = ptr;
            while (*ptr && !strchr(token_delim, *ptr)) {
                ptr++;
            }
            if (*ptr) {
                *ptr = '\0'; // Null terminate the token
                ptr++;
            }
        }

        // Store the token
        tokens[position] = strdup(token_start);
        printf(DEBUG ? "%s[%d]\n" : "", token_start, position);
        position++;

        // Resize token array if needed
        if (position >= buf_size) {
            buf_size += token_buf_size;
            tokens = realloc(tokens, sizeof(char*) * buf_size);
            if (!tokens) {
                fprintf(stderr, "shell: token allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    tokens[position] = NULL; // Null terminate the token array
    return tokens;
}