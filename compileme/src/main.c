#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>

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
        ERROR_ARGUMENTS,
        ERROR_TYPE, 
        ERROR_GENERIC,
        ERROR_SYNTAX,
        ERROR_TODO
    } type;
    char *msg;
} Error;

//lexing

Error ok = { ERROR_NONE, NULL };

void print_error(Error err) {
    if (err.type == ERROR_NONE) {
        return;
    }
    printf("ERROR: ");
    switch (err.type) {
        default:
            printf("Unknown error type...");
            break;
        case ERROR_TODO:
            printf("TODO (not implemented)");
            break;
        case ERROR_SYNTAX:
            printf("invalid syntax");
            break;
        case ERROR_TYPE:
            printf("Mismatched types");
            break;
        case ERROR_ARGUMENTS:
            printf("invalid arguments");
            break;
        case ERROR_NONE:
            break;
        case ERROR_GENERIC:
            break;
    }
    putchar('\n');
    if (err.msg) {
        printf("  : %s\n", err.msg);
    }
}

#define ERROR_CREATE(n, t, msg)         \
    Error(n) = { (t), (msg) }

#define ERROR_PREP(n, t, message)       \
    (n).type = (t);                     \
    (n).msg = (message);

const char *whitespace = " \r\n";
const char *delimiters = " \r\n:";

// lex tokens
Error lex(char *source, char **beg, char **end) {
    Error err = ok;
    if (!source || !beg || !end) {
        ERROR_PREP(err, ERROR_ARGUMENTS, "Can not lex empty source.");
        return err;
    }
    *beg = source;
    *beg += strspn(*beg, whitespace);
    *end = *beg;
    *end += strcspn(*beg, delimiters);
    printf("lexed: %.*s", *end - *beg, *beg);
    return err;
}

Error parse_expr(char *source) {
    char *beg = source;
    char *end = source;
    Error err = ok;
    while ((err = lex(end, &beg, &end)).type == ERROR_NONE) {
        if (end - beg == 0) { break; }
        printf("lexed: %.*s\n", end - beg, beg);
    }
    return err;
}

int main (int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv);
        exit(0);
    }

    char *path = argv[1];
    char *contents = file_contents(path);
    if (contents) {
        printf("Contents of %s: \n---\n\"%s\"\n---\n", path, contents);

        Error err = parse_expr(contents);
        print_error(err);

        free(contents);
    }

    return 0;
}


//1:37:52 part 1