#include "../include/history.h"
#include <stdio.h>
#include <sys/types.h> 

char *history[HISTORY_SIZE];
int history_count = 0;
int history_pos = 0;

void add_to_history(char *command) {
    if (history_count < HISTORY_SIZE) {
        history[history_count++] = strdup(command);
    } else {
        free(history[0]); // Remove the oldest command
        for (int i = 1; i < HISTORY_SIZE; i++) {
            history[i - 1] = history[i];
        }
        history[HISTORY_SIZE - 1] = strdup(command);
    }
    history_pos = history_count;
    save_history();
}
void load_history() {
    FILE *file = fopen(HISTORY_FILE_PATH, "r");
    if (!file) return;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1) {
        line[strcspn(line, "\n")] = '\0'; // Remove newline character
        add_to_history(line);
    }
    free(line);
    fclose(file);
}

void save_history() {
    FILE *file = fopen(HISTORY_FILE_PATH, "w");
    if (!file) return;

    for (int i = 0; i < history_count; i++) {
        fprintf(file, "%s\n", history[i]);
    }
    fclose(file);
}