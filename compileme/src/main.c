#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

long file_size(FILE *file) {
    if (!file) {return 0; }
    fpos_t original = 0;
    if (fgetpos(file, &original) != 0) {
        printf("fgetpos() failed: %i", errno);
        return 0;
    }
    fseek(file, 0, SEEK_END);
    long out = ftell(file);
    if (fsetpos(file, &original) != 0) {
        printf("fsetpos() failed: %i\n", errno);
    }
    return out;
}

char *file_contents(char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        printf("Could not open file at %s\n", path);
        return NULL;
    }
    long size = file_size(file);
    char *contents = malloc(size + 1);
    char *write_it = contents;
    size_t bytes_read = 0;
    while (bytes_read < size) {
        printf("Reading %ld bytes\n", size - bytes_read);
        size_t bytes_read_this_it = fread(contents, 1, size - bytes_read, file);
        if (ferror(file)) {
            printf("Error while reading %i\n", errno);
            free(contents);
            return NULL;
        }

        bytes_read += bytes_read_this_it;
        write_it += bytes_read_this_it;

        if (feof(file)) { 
            break; 
        }
    }
    contents[bytes_read] = '\0';
    return contents;
}

void print_usage(char **argv) {
    printf("USAGE: %s <path_to_file_to_compiler>\n", argv[0]);
}

typedef struct Error {
    enum ErrorType {
        ERROR_NONE = 0,
        ERROR_TYPE, 
        ERROR_GENERIC,
        ERROR_SYNTAX,
        ERROR_TODO
    } type;
    char *msg;
} Error;

//lexing

Error ok = { ERROR_NONE, NULL };

Error lex(char *source, char **beg, char **end) { // beg and end of token
    return ok;
}


//parser

int main (int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv);
        exit(0);
    }

    char *path = argv[1];
    char *contents = file_contents(path);
    if (contents) {
        printf("Contents of %s: \n---\n\"%s\"\n---\n", path, contents);
        free(contents);
    }
    return 0;
}
