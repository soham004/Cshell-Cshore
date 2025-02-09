#include "../include/history.h"
#include "../include/input.h"
#include "../include/builtins.h"
#include "../include/utils.h"

int DEBUG = 0;

void shell_loop() {
    char *line;
    char **args;
    int status = 0;
    char cwd[PATH_MAX];
    do {
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
        }
        line = shell_read_line();
        add_to_history(line);
        printf(DEBUG ? "Line: %s \n" : "", line);
        args = shell_split(line);

        status = shell_execute(args);
        free(line);
        free(args);
    } while (status);
}

int main(int argc, char const *argv[]) {
    if (argc >= 2) {
        if (strcmp(argv[1], "--debug") == 0) {
            DEBUG = 1; // Enable debug mode
        }
    }
    signal(SIGINT, sigint_handler);
    shell_loop();
    
    return EXIT_SUCCESS;
}