#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
#include <ctype.h>

int DEBUG = 0;


#define buffer_size 1024

char *simple_shell_read_line(void){
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

char **simple_shell_split(char *line){
    //this doesn't treat "4 5 6" as a simngle token instead "4, 5 and 6" are three different tokens
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

    
}

void simple_shell_loop(){

    char *line;
    char **args;
    int status = 0;
    char cwd[PATH_MAX];
    do{
        
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s", cwd);
        }
        printf("> ");
        line = simple_shell_read_line();
        printf(DEBUG?"Line: %s \n":"", line);
        args = simple_shell_split(line);
        free(line);
    }while(status);
    

}



int main(int argc, char const *argv[])
{
    if (argc >= 2) {
        if (strcmp(argv[1], "--debug") == 0) {
            DEBUG = 1; // Enable debug mode
        }
    }
    simple_shell_loop();
    
    return EXIT_SUCCESS;
}

