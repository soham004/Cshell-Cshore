#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/wait.h>
#include <ctype.h>

int DEBUG = 0;


#define buffer_size 1024

char *shell_read_line(void){
    int bufferSize = buffer_size;
    int pos=0;
    char *buffer = malloc(sizeof(char)*bufferSize);
    int c;
    if(!buffer){
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE); 
    }
    
    while (1)
    {
        c = getchar();

        if(c == EOF || c == '\n'){
            buffer[pos]='\0';
            return buffer;
        }
        else{
            buffer[pos] = c;
        }
        pos++;

        if(pos>=bufferSize){
            bufferSize += buffer_size;
            buffer = realloc(buffer, bufferSize);
            if(!buffer){
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE); 
            }
        }

    }
    

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
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } 
  else if (pid < 0) {
    // Error forking
    perror("lsh");
  } 
  else {
    // Parent process
    do {
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
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } 
  else {
    printf(DEBUG ? "Changing dir to: %s \n" : "", args[1]);
    if (chdir(args[1]) != 0) {
      perror("lsh");
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
            printf("%s", cwd);
        }
        printf("> ");
        line = shell_read_line();
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
    shell_loop();
    
    return EXIT_SUCCESS;
}

