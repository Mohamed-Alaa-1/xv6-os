#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void write_compressed_sequence(int count, int character) {
    if (fwrite(&count, sizeof(int), 1, stdout) != 1) {
        fprintf(stderr, "wzip: error writing to stdout\n");
        exit(1);
    }
    if (fputc(character, stdout) == EOF) {
        fprintf(stderr, "wzip: error writing to stdout\n");
        exit(1);
    }
}

void compress_file(FILE *fp) {
    int count = 0;
    int current_char;
    int last_char = EOF;
    
    // Handle empty files
    if (feof(fp)) {
        return;
    }

    while ((current_char = fgetc(fp)) != EOF) {
        if (ferror(fp)) {
            fprintf(stderr, "wzip: error reading file\n");
            exit(1);
        }

        if (last_char == EOF) {
            last_char = current_char;
            count = 1;
        } else if (current_char == last_char) {
            count++;
        } else {
            write_compressed_sequence(count, last_char);
            last_char = current_char;
            count = 1;
        }
    }

    // Write the last sequence if exists
    if (last_char != EOF) {
        write_compressed_sequence(count, last_char);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "wzip: file1 [file2 ...]\n");
        return 1;
    }

    // Process all input files
    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "wzip: cannot open file\n");
            return 1;
        }
        compress_file(fp);
        fclose(fp);
    }

    // Ensure all data is written
    if (fflush(stdout) != 0) {
        fprintf(stderr, "wzip: error flushing output\n");
        return 1;
    }

    return 0;
}