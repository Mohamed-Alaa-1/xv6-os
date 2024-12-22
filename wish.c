#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKENS 64
#define DELIMITERS " \t\r\n"
char error_message[30] = "An error has occurred\n";

// Function prototypes
void process_command(char *input);
char **tokenize(char *line);
int is_built_in(char **args);
void execute_built_in(char **args);
void execute_external(char **args);

// Global variables for path
char *search_path[] = {"/bin", NULL};

int main(int argc, char *argv[]) {
    char input[MAX_INPUT_SIZE];
    
    // Check for correct number of arguments
    if (argc > 2) {
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    
    // Interactive mode
    if (argc == 1) {
        while (1) {
            printf("wish> ");
            if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
                exit(0);  // EOF condition
            }
            process_command(input);
        }
    }
    // Batch mode
    else {
        FILE *batch_file = fopen(argv[1], "r");
        if (batch_file == NULL) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
        
        while (fgets(input, MAX_INPUT_SIZE, batch_file) != NULL) {
            process_command(input);
        }
        fclose(batch_file);
        exit(0);
    }
    
    return 0;
}

void process_command(char *input) {
    char **args = tokenize(input);
    if (args[0] == NULL) {  // Empty command
        free(args);
        return;
    }
    
    if (is_built_in(args)) {
        execute_built_in(args);
    } else {
        execute_external(args);
    }
    
    free(args);
}

char **tokenize(char *line) {
    char **tokens = malloc(MAX_TOKENS * sizeof(char*));
    char *token;
    int position = 0;
    
    token = strtok(line, DELIMITERS);
    while (token != NULL && position < MAX_TOKENS - 1) {
        tokens[position] = token;
        position++;
        token = strtok(NULL, DELIMITERS);
    }
    tokens[position] = NULL;
    
    return tokens;
}

int is_built_in(char **args) {
    if (args[0] == NULL) return 0;
    return (strcmp(args[0], "exit") == 0 ||
            strcmp(args[0], "cd") == 0 ||
            strcmp(args[0], "path") == 0);
}

void execute_built_in(char **args) {
    if (strcmp(args[0], "exit") == 0) {
        if (args[1] != NULL) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        exit(0);
    }
    else if (strcmp(args[0], "cd") == 0) {
        if (args[1] == NULL || args[2] != NULL) {
            write(STDERR_FILENO, error_message, strlen(error_message));
            return;
        }
        if (chdir(args[1]) != 0) {
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
    }
}

void execute_external(char **args) {
    pid_t pid = fork();
    
    if (pid < 0) {  // Fork failed
        write(STDERR_FILENO, error_message, strlen(error_message));
        return;
    }
    
    if (pid == 0) {  // Child process
        // Try to execute the command using the search path
        char executable_path[1024];
        int i;
        for (i = 0; search_path[i] != NULL; i++) {
            snprintf(executable_path, sizeof(executable_path), "%s/%s", 
                    search_path[i], args[0]);
            execv(executable_path, args);
        }
        // If we get here, command was not found
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    else {  // Parent process
        wait(NULL);
    }
}
