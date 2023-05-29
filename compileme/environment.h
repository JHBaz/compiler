#ifndef COMPILER_ENVIRONMENT_H
#define COMPILER_ENVIRONMENT_H

//forward declaration ? 
typedef struct Node Node;

//TODO:
// |-- API to create new Binding
//  -- API to add Binding to and exising environment
typedef struct Binding {
    Node *id;
    Node *value;
    struct Binding *next;
} Binding;

typedef struct Environment {
    struct Environment *parent;
    Binding *bind;
} Environment;

Environment *environment_create(Environment *parent);

// @retval 0 failure 
// @retval 1 upon creation of new binding, 
// @retval 2 upon value overwrite (ID unused)
int environment_set (Environment *env, Node *id, Node *value);
/// @ return boolean-like value 1 for success, 0 for failure
int environment_get (Environment env, Node *id, Node *result);
/// @ return boolean-like value 1 for success, 0 for failure
int environment_get_by_symbol(Environment env, char *symbol, Node *result);


#endif /* COMPILER_ENVIRONMENT_H */