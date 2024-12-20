#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void decompress_file(FILE *fp) {
    int count;
    unsigned char character;
    size_t read_size;

    while (1) {
        // Read count (4-byte integer)
        read_size = fread(&count, sizeof(int), 1, fp);
        if (read_size == 0) {
            if (feof(fp)) {
                break;  // Normal end of file
            }
            fprintf(stderr, "wunzip: error reading file\n");
            exit(1);
        }

        // Read character (1 byte)
        read_size = fread(&character, sizeof(char), 1, fp);
        if (read_size != 1) {
            fprintf(stderr, "wunzip: invalid format\n");
            exit(1);
        }

        // Validate count to prevent excessive memory usage
        if (count <= 0) {
            fprintf(stderr, "wunzip: invalid count in compressed file\n");
            exit(1);
        }

        // Print character count times
        for (int i = 0; i < count; i++) {
            if (fputc(character, stdout) == EOF) {
                fprintf(stderr, "wunzip: error writing to stdout\n");
                exit(1);
            }
        }
    }

    // Ensure all data is written
    if (fflush(stdout) != 0) {
        fprintf(stderr, "wunzip: error flushing output\n");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "wunzip: file1 [file2 ...]\n");
        return 1;
    }

    // Process each input file
    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            fprintf(stderr, "wunzip: cannot open file\n");
            return 1;
        }
        decompress_file(fp);
        fclose(fp);
    }

    return 0;
}