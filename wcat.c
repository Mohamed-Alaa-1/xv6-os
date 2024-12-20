#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void print_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "wcat: cannot open file\n");
        exit(1);
    }

    char buffer[4096];  // Larger buffer for better performance
    size_t bytes_read;

    // Handle empty files gracefully
    if (feof(fp)) {
        fclose(fp);
        return;
    }

    // Read and print file contents
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        if (fwrite(buffer, 1, bytes_read, stdout) != bytes_read) {
            fprintf(stderr, "wcat: error writing to stdout\n");
            fclose(fp);
            exit(1);
        }
    }

    // Check for read errors
    if (ferror(fp)) {
        fprintf(stderr, "wcat: error reading file\n");
        fclose(fp);
        exit(1);
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {
    // No arguments is valid - just exit
    if (argc == 1) {
        return 0;
    }

    // Process each file
    for (int i = 1; i < argc; i++) {
        print_file(argv[i]);
    }

    return 0;
}