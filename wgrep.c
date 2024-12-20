#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// For systems that might not have ssize_t
#ifndef ssize_t
#define ssize_t long
#endif

void grep_file(FILE *fp, const char *searchterm) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    
    // Handle empty search term
    if (strlen(searchterm) == 0) {
        return;
    }

    errno = 0;  // Reset errno before getline calls

    while ((read = getline(&line, &len, fp)) != -1) {
        // Check for line reading errors
        if (read == -1 && errno != 0) {
            fprintf(stderr, "wgrep: error reading file\n");
            free(line);
            exit(1);
        }

        // Search and print matching lines
        if (strstr(line, searchterm) != NULL) {
            if (fputs(line, stdout) == EOF) {
                fprintf(stderr, "wgrep: error writing to stdout\n");
                free(line);
                exit(1);
            }
        }
    }

    free(line);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "wgrep: searchterm [file ...]\n");
        return 1;
    }

    const char *searchterm = argv[1];

    // Read from stdin if no files specified
    if (argc == 2) {
        grep_file(stdin, searchterm);
        return 0;
    }

    // Process each file
    for (int i = 2; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "wgrep: cannot open file\n");
            return 1;
        }
        grep_file(fp, searchterm);
        fclose(fp);
    }

    return 0;
}