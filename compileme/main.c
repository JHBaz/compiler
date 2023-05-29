#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "file_io.h"
#include "parser.h"
#include "environment.h"

void print_usage(char **argv) {
    printf("USAGE: %s <path_to_file_to_compiler>\n", argv[0]);
}

int main (int argc, char **argv) 
{
    if (argc < 2) {
        print_usage(argv);
        exit(0);
    }

    char *path = argv[1];
    char *contents = file_contents(path);

    if (contents) {
        //printf("Contents of %s: \n---\n\"%s\"\n---\n", path, contents);

        // TODO: Create API to heap allocate program node
        // as well as add expressions as children
        ParsingContext *context = parse_context_create();
        Node *program = node_allocate();
        program->type = NODE_TYPE_PROGRAM;
        Node *expression = node_allocate();
        memset(expression,0,sizeof(Node));
        char *contents_it = contents;
        Error err = parse_expr(context, contents_it, &contents_it, expression);
        node_add_child(program, expression);
        putchar('\n');

        print_error(err);
        
        print_node(program, 0);
        putchar('\n');

        node_free(program);
        free(contents);
    }
    return 0;
}

// 5