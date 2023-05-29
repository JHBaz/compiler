#include "parser.h"
#include "error.h"
#include "environment.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ================================================================================= BEG lexer

const char *whitespace = " \r\n";
const char *delimiters = " \r\n,():";

void print_token(Token t) {
    printf("%.*s", t.end - t.beginning, t.beginning);
}

// lex the next token from source and point to it eith begin and end
Error lex(char *source, Token *token) {
    Error err = ok;
    if (!source || !token) {
        ERROR_PREP(err, ERROR_ARGUMENTS, "Can not lex empty source.");
        return err;
    }
    token->beginning = source;
    token->beginning += strspn(token->beginning, whitespace);
    token->end = token->beginning;
    if (*(token->end) == '\0') { return err; }
    token->end += strcspn(token->beginning, delimiters);
    if (token->end == token->beginning) {
        token->end += 1;
    }
    return err;
}

//@return Boolean-like value; 1 succ, 0 fail
int token_string_equalp(char* string, Token *token) {
    if (!string || !token->beginning || !token->end) { return 0;}
    char *beg = token->beginning;
    while(*string && token->beginning < token->end) {
        if (*string != *beg) {
            return 0;
        }
        string++;
        beg++;
    }
    return 1;
}

// ================================================================================= END lexer
Node *node_allocate() {
    Node *node = calloc(1, sizeof(Node));
    assert(node && " Could not allocate memory for AST node");
    return node;
}

void node_add_child(Node *parent, Node *new_child) {
    if (!parent || !new_child) { return; }
    if (parent->children) {
        Node *child = parent->children;
        while (child->next_child) {
            child = child->next_child;
        }
        child->next_child = new_child;
    } else {
        parent->children = new_child;
    }
}

int node_compare(Node *a, Node *b) {
    if (!a || !b) {
        if (!a && !b) {
            return 1;
        }
        return 0;
    }
    // assert is broken and im not sure why 
    assert(NODE_TYPE_MAX == 7 && "node_compare() must handle all node types");
    if(a->type != b->type) { return 0; }
    switch (a->type) {
    case NODE_TYPE_NONE:
        if (nonep(*b)) {
            return 1;
        }
        return 0;
        break;
    case NODE_TYPE_INTEGER:
        if (a->value.integer == b->value.integer) {
            return 1;
        }
        return 0;
        break;
    case NODE_TYPE_SYMBOL:
        if(a->value.symbol && b->value.symbol) {
            if (strcmp(a->value.symbol, b->value.symbol) == 0) {
                return 1;
            }
            return 0;
        } else if (!a->value.symbol && !b->value.symbol) {
            return 1;
        }
        return 0;
        break;
    case NODE_TYPE_BINARAY_OPERATOR:
        printf("TODO: nodecompare() bin op");
        break;
    case NODE_TYPE_VARIABLE_DECLARATION:
        printf("TODO: nodecompare() NODE_TYPE_VARIABLE_DECLARATION");
        break;
    case NODE_TYPE_VARIABLE_DECLARATION_INTIALISED:
        printf("TODO: nodecompare() NODE_TYPE_VARIABLE_DECLARATION_INTIALISED");
        break;
    case NODE_TYPE_PROGRAM:
        //COMPARE two programs
        printf("TODO: compare two programs");
        break;
    }
    return 0;
}

Node *node_integer(long long value) {
    Node *integer = node_allocate();
    integer->type = NODE_TYPE_INTEGER;
    integer->value.integer = value;
    integer->children = NULL;
    integer->next_child = NULL;
    return integer;
}

Node *node_symbol(char *symbol_string) {
    Node *symbol = node_allocate();
    symbol->type = NODE_TYPE_SYMBOL;
    symbol->value.symbol = strdup(symbol_string);
    symbol->children = NULL;
    symbol->next_child = NULL;
    return symbol;
}

Node *node_symbol_from_buffer(char *buffer, size_t length) {
assert(buffer && " Can not create symbol node from NULL buffer");
    char *symbol_string = malloc(length + 1);
    assert(symbol_string && "Could not allocate memory for symbol");
    memcpy(symbol_string, buffer, length);
    symbol_string[length] = '\0';
    Node *symbol = node_allocate();
    symbol->type = NODE_TYPE_SYMBOL;
    symbol->value.symbol = symbol_string;
    return symbol;
}


void print_node(Node *node, size_t indent_level) {
    if (!node) { return; }
    //print indent
    for (size_t i = 0; i < indent_level; ++i) {
        putchar(' ');
    }
    //print type + value
    assert(NODE_TYPE_MAX == 7 && "print_node() must handle all node types");
    switch (node->type) {
        default:
            printf("UNKNOWN");
            break;
        case NODE_TYPE_NONE:
            printf("NONE");
            break;
        case NODE_TYPE_INTEGER:
            printf("INT:%lld", node->value.integer);
            break;
        case NODE_TYPE_SYMBOL:
            printf("SYM");
            if (node->value.symbol) {
                printf(":%s", node->value.symbol);
            }
            break;
        case NODE_TYPE_BINARAY_OPERATOR:
            printf("TODO: print_node() BINARY_OPERATOR");
            break;
        case NODE_TYPE_VARIABLE_DECLARATION:
            printf("VARIABLE_DECLARATION");
            //printf("VAR_DECL:");
            //TODO: Print first child (ID symbol), then type of second child. 
            break;
        case NODE_TYPE_VARIABLE_DECLARATION_INTIALISED:
            printf("TODO: print_node() VARIABLE_DECLARATION_INTIALISED");
            break;
        case NODE_TYPE_PROGRAM:
            printf("PROGRAM");
            break;
    }
    putchar('\n');
    //print children
    Node *child = node->children;
    while(child) {
        print_node(child, indent_level + 4);
        child = child->next_child;
    }
}

// make more efficient, maybe keeping track of allocated pointers then free all in one go?
void node_free(Node *root) {
    if (!root) { return; }
    Node *child = root->children;
    Node *next_child = NULL;
    while(child) {
        next_child = child->next_child;
        node_free(child);
        child = next_child;
    }
    if (symbolp(*root) && root->value.symbol) {
        free(root->value.symbol);    
    }
    free(root);
}

ParsingContext *parse_context_create() {
    ParsingContext *ctx = calloc(1, sizeof(ParsingContext));
    assert(ctx && "Could not allocate memory for parsing context");
    ctx->types = environment_create(NULL);

    if ((environment_set(ctx->types, node_symbol("integer"), node_integer(69))) == 0) {
        printf("ERROR: Failed to set builtin type in environment");
    }
    ctx->variables = environment_create(NULL);
    return ctx;
}

//@return boolean-like value; 1 succ 0 fail
int parse_integer(Token *token, Node *node) {
    if (!token || !node) {return 0 ;}
    char *end = NULL;
    if (token->end - token->beginning == 1 && *(token->beginning) == '0') { 
        node->type = NODE_TYPE_INTEGER;
        node->value.integer = 0;
    } else if ((node->value.integer = strtoll(token->beginning, NULL, 10)) != 0) { 
        if (end != token->end) {
            return 0;
        }
        node->type = NODE_TYPE_INTEGER;
    } else { return 0; }
    return 1;
}

Error parse_expr
(ParsingContext *context, 
    char *source, 
    char **end, 
    Node *result) 
    {
    size_t token_count = 0;
    Token current_token;
    current_token.beginning = source;
    current_token.end       = source;
    Error err = ok;

    while ((err = lex(current_token.end, &current_token)).type == ERROR_NONE) {
        *end = current_token.end;
        size_t token_length = current_token.end - current_token.beginning;
        if (token_length == 0) { break; }
        if (parse_integer(&current_token, result)) {
            // look ahead for bin ops that inc ints
            Node lhs_integer = *result;
            err = lex(current_token.end, &current_token);
            if (err.type != ERROR_NONE) {
                return err;
            }
            *end = current_token.end;
            // TODO: look for valid integer operator
            // moperator environment to look up 
            // operators instead of hard coding them
            // user defined operators?

        } else {

            //todo: check for unary prefix  operatrors

            // TODO: check that it iusnt aq bindary operator
            // shouldnt encounter left side first and peek forward, rathewr than encounter it at top level


            Node *symbol = node_symbol_from_buffer(current_token.beginning, token_length);

            *result = *symbol;

            // todo: check if valid symbol for variable envcironemnt and then
            // attempt variable access, assignment, delaration or declaration withi init

            err = lex(current_token.end, &current_token);
            if (err.type != ERROR_NONE) { return err; }
            *end = current_token.end;
            size_t token_length = current_token.end - current_token.beginning;
            if (token_length == 0) { break; }
            
            if (token_string_equalp(":", &current_token)) {
                err = lex(current_token.end, &current_token);
                if (err.type != ERROR_NONE) { return err; }
                *end = current_token.end;
                size_t token_length = current_token.end - current_token.beginning;
                if (token_length == 0) { break; }
                
                Node *expected_type_symbol = 
                    node_symbol_from_buffer(current_token.beginning, token_length);
                if (environment_get(*context->types,expected_type_symbol, result) == 0) {
                    ERROR_PREP(err, ERROR_TYPE, "Invalid type within variable devlaration");
                    printf("\nINVALID TYPE: \"%s\"\n", expected_type_symbol->value.symbol);
                    print_node(expected_type_symbol, 0);
                    return err;
                } else {
                    // printf("Found valid type: ");
                    // print_node(expected_type_symbol, 0);
                    // putchar('\n');

                    Node *var_decl = node_allocate();
                    var_decl->type = NODE_TYPE_VARIABLE_DECLARATION;

                    Node *type_node = node_allocate();
                    type_node->type = NODE_TYPE_INTEGER;

                    node_add_child(var_decl, type_node);
                    node_add_child(var_decl, symbol);

                    *result = *var_decl;
                    // Node contents transfer ownership, var_decl is now hollow shell. 
                    free(var_decl);

                    return ok;
                }
            }

            printf("Unrecognised token: ");
            print_token(current_token);
            putchar('\n');

            return err;
        }

        printf("Intermediate node: ");
        print_node(result, 0);
        putchar('\n');

    }
    return err;
}


