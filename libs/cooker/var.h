/* The Cook project
 * The Cook variables.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __VAR_H__
#define __VAR_H__

#include "strvec.h"
#include "atom.h"

/// A vector of var_t's
typedef vector_t vector_var_t;

/**
 * Initialize a vector of var_t objects.
 * Automatic free & sorting is implemented.
 *
 * @param vector The vector to initialize.
 */
extern void vector_var_init (vector_var_t *vector);

// ---------- // ---------- // ---------- // ---------- // ---------- //

/**
 * Cook variables are complex objects.
 * They have a name, and an assotiated value and dictionary.
 * The value is really a list of values, and dictionary is
 * a name -> variable map.
 */
typedef struct
{
    /// The name of the variable
    str_t name;
    /// A list of word_t values
    vector_atom_t value;
    /// The variable fields
    vector_var_t fields;
    /// A pointer to parent variable (or NULL for root context)
    struct var_t *parent;
} var_t;

/**
 * Initialize a variable to empty state.
 *
 * @param var The variable to initialize.
 */
extern void var_init (var_t *var, str_t *name);

/**
 * Finalize a variable object.
 *
 * @param var The object to finalize.
 */
extern void var_done (var_t *var);

/**
 * Finalize a variable object and free the memory for the structure.
 *
 * @param var A pointer to the object to finalize and free.
 */
extern void var_free (var_t *var);

// ---------- // ---------- // ---------- // ---------- // ---------- //

/**
 * Get a pointer to the root context. If the context has not been created
 * yet, the function will initialize it to a empty state.
 *
 * @return The root context.
 */
extern var_t *var_get_root_ctx ();

/**
 * Free the rooot context. This will destroy the whole tree database of
 * variables.
 */
extern void var_done_root_ctx ();

#endif /* __VAR_H__ */
