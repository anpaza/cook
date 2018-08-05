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

void atom_init (atom_t *atom, atom_type_t type)
{
    assert (atom);

    memset (atom, 0, sizeof (*atom));

    atom->type = type;
}

atom_t *atom_new (atom_type_t type)
{
    atom_t *ret = malloc (sizeof (atom_t));
    if (ret)
        atom_init (ret, type);

    return ret;
}

atom_t *atom_new_copy (atom_t *atom)
{
    atom_t *ret = malloc (sizeof (atom_t));
    if (ret)
    {
        atom_init (ret, atom->type);
        ret->adjoin = atom->adjoin;
    }

    return ret;
}

void atom_done (atom_t *atom)
{
    assert (atom);
    assert (atom->vmt);

    atom->vmt->done (atom);
}

void atom_free (atom_t *atom)
{
    atom_done (atom);
    free (atom);
}

// ---------- // ---------- // ---------- // ---------- // ---------- //

static void vector_atom_free (void *item);

static vector_vmt_t vector_atom_vmt =
{
    vector_atom_free,
    NULL,
    NULL,
};

static void vector_atom_free (void *item)
{
    atom_free (item);
}

void vector_atom_init (vector_t *vecw)
{
    vector_init (vecw, 0);
    vecw->vmt = &vector_atom_vmt;
}
