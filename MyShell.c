#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include <termios.h>


int DEBUG = 0;

#define HISTORY_SIZE 20
char *history[HISTORY_SIZE];
int history_count = 0;
int history_pos = 0;

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

#define buffer_size 1024
char *shell_read_line(void) {
    enableRawMode();

    int bufferSize = buffer_size, pos = 0;
    char *buffer = malloc(bufferSize);
    char cwd[PATH_MAX];
    int c;

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
                    if (history_count > 0) {
                        history_pos = (history_pos - 1 + history_count) % history_count;
                        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                            printf("\r\033[K%s> ", cwd);// Clear line
                        }
                        printf("%s", history[history_pos]); 
                        strcpy(buffer, history[history_pos]);
                        pos = strlen(buffer);
                    }
                    continue;
                case 'B': // Down Arrow
                    if (history_count > 0) {
                        history_pos = (history_pos + 1) % history_count;
                        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                            printf("\r\033[K%s> ", cwd);
                        }
                        printf("%s", history[history_pos]); // Print next history entry
                        strcpy(buffer, history[history_pos]);
                        pos = strlen(buffer);
                    }
                    continue;
            }
        }

        if (c == '\n'|| c==EOF) {
            printf("\n");
            buffer[pos] = '\0';
            break;
        } 
        else if (c == 127) { // Handle Backspace
            if (pos > 0) {
                printf("\b \b"); // Erase character
                pos--;
            }
        } 
        else {
            buffer[pos++] = c;
            printf("%c", c); // Echo character
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
}


#define token_buf_size 64
#define token_delim " \t\n\r\a"

char **shell_split(char *line){
    // this doesn't treat "4 5 6" as a simngle token instead "4, 5 and 6" are three different tokens
    /*
    int buf_size = token_buf_size, position = 0;
    char **tokens = malloc(sizeof(char*)*buf_size);
    char *token;
    if(!tokens){
        fprintf(stderr, "shell: token allocation error\n");
        exit(EXIT_FAILURE);
    }
    token = strtok(line, token_delim);
    printf(DEBUG?"Args: \n":"");
    while(token != NULL){
        printf(DEBUG?"%s \n":"", token);
        tokens[position];
        position++;
        if(position >= buf_size){
            buf_size += token_buf_size;
            tokens = realloc(tokens,sizeof(char*)*buf_size);
        }
        token = strtok(NULL, token_delim);
    }
    tokens[position] = NULL;
    return tokens;
    */
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
        } else {
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
        printf(DEBUG ? "%s[%d]\n" : "", token_start,position);
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

int shell_launch(char **args)
{
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
  } 
  else if (pid < 0) {
    // Error forking
    perror("Error Forking");
  } 
  else {
    // Parent process
    do {
        printf(DEBUG ? "Main Process: Waiting for child process\n" : "");
        wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}


int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

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

int shell_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "Expected argument to \"cd\"\n");
  } 
  else {
    printf(DEBUG ? "Changing dir to: %s \n" : "", args[1]);
    if (chdir(args[1]) != 0) {
      perror("Couldn't change directory");
    }
  }
  return 1;
}

int shell_help(char **args)
{
  int i;
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < shell_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int shell_exit(char **args)
{
    printf(DEBUG ? "Received exit command\n" : "");
    return 0;
}

int shell_execute(char **args)
{
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

void shell_loop(){

    char *line;
    char **args;
    int status = 0;
    char cwd[PATH_MAX];
    do{
        
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
        }
        line = shell_read_line();
        add_to_history(line);
        printf(DEBUG?"Line: %s \n":"", line);
        args = shell_split(line);

        status = shell_execute(args);
        free(line);
        free(args);
    }while(status);
    

}

int main(int argc, char const *argv[])
{
    if (argc >= 2) {
        if (strcmp(argv[1], "--debug") == 0) {
            DEBUG = 1; // Enable debug mode
        }
    }
    signal(SIGINT, sigint_handler);
    shell_loop();
    
    return EXIT_SUCCESS;
}

