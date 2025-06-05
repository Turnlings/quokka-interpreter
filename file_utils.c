#include <stdio.h>
#include <stdlib.h>

// Reads an entire multiline file into a single string
char* readFile(char* name) {
    FILE *fptr = fopen(name, "r");
    if (!fptr) {
        fprintf(stderr, "Failed to open: %s\n", name);
        return NULL;
    }

    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    rewind(fptr);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(fptr);
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    size_t read_size = fread(buffer, 1, size, fptr);
    if (read_size != size) {
        fclose(fptr);
        free(buffer);
        fprintf(stderr, "Failed to read file\n");
        return NULL;
    }

    buffer[size] = '\0'; // Null terminate
    fclose(fptr);
    return buffer;
}