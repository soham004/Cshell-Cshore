#include "../include/builtins.h"
#include "../include/utils.h"

char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int (*builtin_func[]) (char **) = {
    &shell_cd,
    &shell_help,
    &shell_exit
};

int shell_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int shell_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Expected argument to \"cd\"\n");
    } else {
        printf(DEBUG ? "Changing dir to: %s \n" : "", args[1]);
        if (chdir(args[1]) != 0) {
            perror("Couldn't change directory");
        }
    }
    return 1;
}

int shell_help(char **args) {
    int i;
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for (i = 0; i < shell_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other programs.\n");
    return 1;
}

int shell_exit(char **args) {
    printf(DEBUG ? "Received exit command\n" : "");
    return 0;
}

int shell_execute(char **args) {
    int i;

    if (args[0] == NULL) {
        // An empty command was entered.
        return 1;
    }

    for (i = 0; i < shell_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return shell_launch(args);
}