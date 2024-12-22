#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_PATHS 100
#define MAX_ARGS 100

char *paths[MAX_PATHS]; // Array to hold paths for executables
int path_count = 1;     // Number of paths in the array

void print_error() {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void initialize_paths() {
    paths[0] = "/bin"; // Default path
    path_count = 1;
}

void set_paths(char **args, int arg_count) {
    path_count = 0; // Reset paths
    for (int i = 1; i < arg_count; i++) {
        paths[path_count++] = strdup(args[i]);
    }
}

void execute_command(char **args, int redirect, char *filename) {
    char full_path[1024];
    for (int i = 0; i < path_count; i++) {
        snprintf(full_path, sizeof(full_path), "%s/%s", paths[i], args[0]);
        if (access(full_path, X_OK) == 0) {
            if (fork() == 0) {
                if (redirect) {
                    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
                    if (fd < 0) {
                        print_error();
                        exit(1);
                    }
                    dup2(fd, STDOUT_FILENO);
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }
                execv(full_path, args);
                exit(0);
            }
            wait(NULL);
            return;
        }
    }
    print_error();
}

void handle_line(char *line) {
    char *commands[100];
    int command_count = 0;

    // Split line into commands (parallel execution)
    char *command = strtok(line, "&");
    while (command) {
        commands[command_count++] = strdup(command);
        command = strtok(NULL, "&");
    }

    for (int i = 0; i < command_count; i++) {
        if (fork() == 0) {
            // Process individual command
            char *args[MAX_ARGS];
            int arg_count = 0;
            char *token = strtok(commands[i], " \t\n");
            int redirect = 0;
            char *filename = NULL;

            while (token) {
                if (strcmp(token, ">") == 0) {
                    redirect = 1;
                    token = strtok(NULL, " \t\n");
                    if (token) filename = strdup(token);
                    break;
                }
                args[arg_count++] = strdup(token);
                token = strtok(NULL, " \t\n");
            }
            args[arg_count] = NULL;

            if (arg_count == 0) {
                exit(0); // Empty command
            }

            // Handle built-in commands
            if (strcmp(args[0], "exit") == 0) {
                if (arg_count != 1) {
                    print_error();
                } else {
                    exit(0);
                }
            } else if (strcmp(args[0], "cd") == 0) {
                if (arg_count != 2 || chdir(args[1]) != 0) {
                    print_error();
                }
                exit(0);
            } else if (strcmp(args[0], "path") == 0) {
                set_paths(args, arg_count);
                exit(0);
            }

            // Execute other commands
            execute_command(args, redirect, filename);
            exit(0);
        }
    }

    // Wait for all parallel processes
    for (int i = 0; i < command_count; i++) {
        wait(NULL);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        print_error();
        exit(1);
    }

    FILE *input = (argc == 2) ? fopen(argv[1], "r") : stdin;
    if (input == NULL) {
        print_error();
        exit(1);
    }

    initialize_paths();

    char *line = NULL;
    size_t len = 0;

    while (1) {
        if (argc == 1) printf("wish> ");
        if (getline(&line, &len, input) == -1) {
            free(line);
            exit(0);
        }
        handle_line(line);
    }

    free(line);
    fclose(input);
    return 0;
}
