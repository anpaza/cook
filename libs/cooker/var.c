/* The Cook project
 * The variables.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "var.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

void var_init (var_t *var, str_t *name)
{
    assert (var);

    str_init_copy (&var->name, name);
    vector_atom_init (&var->value);
    vector_var_init (&var->fields);
}

void var_done (var_t *var)
{
    assert (var);

    str_done (&var->name);
    vector_done (&var->value);
    vector_done (&var->fields);
}

void var_free (var_t *var)
{
    assert (var);

    var_done (var);
    free (var);
}

// ---------- // ---------- // ---------- // ---------- // ---------- //

static void vector_var_free (void *item);
static int vector_var_compare (const void *item1, const void *item2);
static int vector_var_compare_key (const void *item, const void *key);

static vector_vmt_t vector_var_vmt =
{
    vector_var_free,
    vector_var_compare,
    vector_var_compare_key,
};

static void vector_var_free (void *item)
{
    var_free (item);
}

static int vector_var_compare (const void *item1, const void *item2)
{
    return str_cmp (&((var_t *)item1)->name, &((var_t *)item2)->name);
}

static int vector_var_compare_key (const void *item, const void *key)
{
    return str_cmp (&((var_t *)item)->name, (str_t *)key);
}

void vector_var_init (vector_t *vector)
{
    vector_init (vector, 0);
    vector->vmt = &vector_var_vmt;
}

// ---------- // ---------- // ---------- // ---------- // ---------- //

static const str_t ctx_root_ctx_name = STR_INIT_C ("");
// The root context
static var_t cook_ctx_root;

var_t *var_get_root_ctx ()
{
    // If root context is uninitialized yet, initialize it now
    if (!cook_ctx_root.name.data)
        var_init (&cook_ctx_root, (str_t *)&ctx_root_ctx_name);

    return &cook_ctx_root;
}

void var_done_root_ctx ()
{
    if (cook_ctx_root.name.data)
        var_done (&cook_ctx_root);

    memset (&cook_ctx_root, 0, sizeof (cook_ctx_root));
}
