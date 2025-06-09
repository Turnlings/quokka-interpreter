#include <stdio.h>
#include <stdlib.h>

/**
 * read_file - Reads all the text in a given file
 * 
 * @param name The filename to read from
 * 
 * @return The string of characters in the file
 */
char* read_file(char* name) {
    FILE *fptr = fopen(name, "r");
    if (!fptr) {
        fprintf(stderr, "Failed to open: %s\n", name);
        return NULL;
    }

    // Skip to end to find the length of the file
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

    buffer[size] = '\0'; // Add null terminate
    fclose(fptr);
    return buffer;
}

/**
 * write_file - Writes text into the given file
 * 
 * @param name The filename of the file to write to
 * @param text The text to write into the file
 * 
 * @return 0 if successful, else an error code
 */
int write_file(char *name, char *text) {
    FILE *file = fopen(name, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return 1;
    }

    fputs(text, file);

    fclose(file); 

    return 0;
}