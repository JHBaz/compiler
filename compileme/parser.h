#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <stddef.h>
#include "error.h"

typedef struct Environment Environment; 

typedef struct Token {
    char *beginning;
    char *end;
    struct Token *next;
} Token;

void print_token(Token t);
Error lex(char *source, Token *token);

typedef struct Node {
    // TODO: how to document node types nd how they fiot in the AST 
    enum NodeType {
        NODE_TYPE_NONE,

        /// Just an Integer
        NODE_TYPE_INTEGER,

        /// Anything that isnt another literal type becomes a symbol
        NODE_TYPE_SYMBOL,

        NODE_TYPE_VARIABLE_DECLARATION,
        NODE_TYPE_VARIABLE_DECLARATION_INTIALISED,
        
        NODE_TYPE_BINARAY_OPERATOR,
        NODE_TYPE_PROGRAM,
        NODE_TYPE_MAX
    } type;
    union NodeValue {
        long long integer;
        char *symbol;
    } value; 
    // maybe parent?
    struct Node *children;
    struct Node *next_child;
} Node;

Node *node_allocate();

#define nonep(node)     ((node).type == NODE_TYPE_NONE)
#define integerp(node)  ((node).type == NODE_TYPE_INTEGER)
#define symbolp(node)   ((node).type == NODE_TYPE_SYMBOL)

/// PARENT is modified, NEW_CHILD pointer is used verbatim  
void node_add_child(Node *parent, Node *new_child);

/// @return Boolean-like value; 1 for succ, 0 for failure
int node_compare(Node *a, Node *b);

/// create a new node with integer type and given value.
Node *node_integer(long long value);

// TODO: think about chacxhing used symbols and not creating dups
/// Create a new node with a symbol type and given value
Node *node_symbol(char *symbol_string);

/// Create a new node with symbol type and value copied from given buffer.
Node *node_symbol_from_buffer(char *buffer, size_t length);

void print_node(Node *node, size_t indent_level);

void node_free(Node *root);

//@return Boolean-like value; 1 succ, 0 fail
int token_string_equalp(char* string, Token *token);

//@return boolean-like value; 1 succ 0 fail
int parse_integer(Token *token, Node *node);

typedef struct ParsingContext {
    // struct ParsingContenxt *parent; -- maybe have parent here so do not need it in environment?
    Environment *types;
    Environment *variables;
} ParsingContext;

ParsingContext *parse_context_create();

Error parse_expr
(ParsingContext *context, 
    char *source, 
    char **end, 
    Node *result);

#endif /* COMPILER_PARSER_H */