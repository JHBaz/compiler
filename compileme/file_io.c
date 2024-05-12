#include "file_io.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

long file_size(FILE *file) {
    fseek(file, 0, SEEK_END); // Move the file pointer to the end of the file.
    long size = ftell(file);  // Get the current position of the file pointer (end of file).
    rewind(file);             // Reset the file pointer to the beginning of the file.
    return size;
}

char *file_contents(char *path) {
    FILE *file = NULL;
    errno_t err = fopen_s(&file, path, "r");
    if (err != 0 || file == NULL) {
        printf("Could not open file at %s\n", path);
        return NULL;
    }

    long size = file_size(file);
    char *contents = malloc(size + 1);
    assert(contents && "Could not allocate buffer for file contents");

    size_t bytes_read = fread(contents, 1, size, file);
    contents[bytes_read] = '\0'; // Null-terminate the string.

    if (ferror(file)) {
        printf("Error while reading %i\n", errno);
        free(contents);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return contents;
}