#include "../include/utils.h"

void sigint_handler(int signo) {
    char cwd[PATH_MAX];

    printf("\n");  // Move to a new line
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s> ", cwd);
    }  // Print new prompt
    fflush(stdout); // Ensure prompt is displayed immediately
}

void enableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);  // Get current terminal attributes
    raw.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); // Apply changes
}

void disableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ICANON | ECHO); // Restore original settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int shell_launch(char **args) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        printf(DEBUG ? "Launched Child Process\n" : "");
        signal(SIGINT, SIG_DFL);
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("Error Executing");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror("Error Forking");
    } else {
        // Parent process
        do {
            printf(DEBUG ? "Main Process: Waiting for child process\n" : "");
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}