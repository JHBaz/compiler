#include "environment.h"
#include "parser.h"

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>


Environment *environment_create(Environment *parent) {
    Environment *env = malloc(sizeof(Environment));
    assert(env && "Could not allocate memory for new environment");
    env->parent = parent;
    env->bind = NULL;
    return env;
}

// @retval 0 failure 
// @retval 1 upon creation of new binding, 
// @retval 2 upon value overwrite (ID unused)
int environment_set (Environment *env, Node *id, Node *value) {
    // overwrite existing value if ID is already bound in environment
    if (!env || !id || !value) {
        return 0;
    }

    Binding *binding_it = env->bind;
    while (binding_it) {
        if (node_compare(binding_it->id, id)) {
            binding_it->value = value;
            node_free(id);
            return 2;
        }
        binding_it = binding_it->next;
    }
    //create new binding
    Binding *binding = malloc(sizeof(Binding));
    assert(binding && "Could not allocate new binding for environment");
    binding->id = id;
    binding->value = value;
    binding->next = env->bind;
    env->bind = binding;
    return 1;
}

/// @ return boolean-like value 1 for success, 0 for failure
int environment_get (Environment env, Node *id, Node *result) {
    Binding *binding_it = env.bind;
    while (binding_it) {
        if (node_compare(binding_it->id, id)) {
            *result = *binding_it->value;
            return 1;
        }
        binding_it = binding_it->next;
    }
    return 0;
}

int environment_get_by_symbol(Environment env, char *symbol, Node *result) {
    Node *symbol_node = node_symbol(symbol);
    int status = environment_get(env, symbol_node, result);
    free(symbol_node);
    return status;
    
}