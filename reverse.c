#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct Node {
    char *line;
    struct Node *next;
};

// Function prototypes
void add_line(struct Node **head, char *line);
void write_output(struct Node *head, FILE *output);
void free_list(struct Node *head);
char *trim_newline(char *str);

int main(int argc, char *argv[]) {
    FILE *input = stdin;
    FILE *output = stdout;
    struct Node *head = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // Check for too many arguments
    if (argc > 3) {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    // Handle input file
    if (argc >= 2) {
        input = fopen(argv[1], "r");
        if (!input) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]);
            exit(1);
        }
    }

    // Handle output file
    if (argc == 3) {
        if (strcmp(argv[1], argv[2]) == 0) {
            fprintf(stderr, "reverse: input and output file must differ\n");
            if (input != stdin) fclose(input);
            exit(1);
        }
        output = fopen(argv[2], "w");
        if (!output) {
            fprintf(stderr, "reverse: cannot open file '%s'\n", argv[2]);
            if (input != stdin) fclose(input);
            exit(1);
        }

        // Check if input and output are the same file (hardlinked)
        if (input != stdin && output != stdout) {
            int fd_in = fileno(input);
            int fd_out = fileno(output);
            struct stat stat_in, stat_out;

            if (fstat(fd_in, &stat_in) == 0 && fstat(fd_out, &stat_out) == 0) {
                if (stat_in.st_dev == stat_out.st_dev && stat_in.st_ino == stat_out.st_ino) {
                    fprintf(stderr, "reverse: input and output file must differ\n");
                    fclose(input);
                    fclose(output);
                    exit(1);
                }
            }
        }
    }

    // Read all lines
    while ((read = getline(&line, &len, input)) != -1) {
        char *line_copy = strdup(line);
        if (!line_copy) {
            fprintf(stderr, "malloc failed\n");
            exit(1);
        }
        // Remove trailing newline for consistent handling
        trim_newline(line_copy);
        add_line(&head, line_copy);
    }

    // Write output in reverse order
    write_output(head, output);

    // Cleanup
    free(line);
    free_list(head);
    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);

    exit(0); // Changed exit(1) to exit(0) for successful execution
}

// Remove trailing newline if it exists
char *trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len-1] == '\n') {
        str[len-1] = '\0';
    }
    return str;
}

void add_line(struct Node **head, char *line) {
    struct Node *new_node = malloc(sizeof(struct Node));
    if (!new_node) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
    }
    new_node->line = line;
    new_node->next = *head;
    *head = new_node;
}
void write_output(struct Node *head, FILE *output) {
    if (!head) return;  // Handle empty file case
    // First line doesn't need a leading newline
    fprintf(output, "%s", head->line);

    // Subsequent lines need leading newlines
    struct Node *current = head->next;
    while (current != NULL) {
        fprintf(output, "\n%s", current->line);
        current = current->next;
    }
}
void free_list(struct Node *head) {
    while (head) {
        struct Node *temp = head;
        head = head->next;
        free(temp->line);
        free(temp);
    }
}
