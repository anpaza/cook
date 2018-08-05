/* The Cook project
 * A sortable vector of strings.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "strvec.h"

#include <assert.h>

static void vector_str_free (void *item);
static int vector_str_compare (const void *item1, const void *item2);
static int vector_str_compare_key (const void *item, const void *key);

static vector_vmt_t vector_str_vmt =
{
    vector_str_free,
    vector_str_compare,
    vector_str_compare_key,
};

void vector_str_init (vector_t *vec, int size)
{
    assert (vec);

    vector_init (vec, size);
    vec->vmt = &vector_str_vmt;
}

vector_t *vector_str_new (int size)
{
    vector_t *vec = vector_new (size);
    if (vec)
        vec->vmt = &vector_str_vmt;

    return vec;
}

static void vector_str_free (void *item)
{
    str_free (item);
}

static int vector_str_compare (const void *item1, const void *item2)
{
    return str_cmp (item1, item2);
}

static int vector_str_compare_key (const void *item, const void *key)
{
    return str_cmp (item, (str_t *)key);
}
