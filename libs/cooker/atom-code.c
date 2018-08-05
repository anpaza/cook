/* The Cook project
 * Atoms containing virtual processor code.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "atom-code.h"

#include <stdlib.h>

static void atom_code_text (atom_t *atom, str_t *str);

static atom_vmt_t atom_code_vmt =
{
    (void (*) (atom_t *atom))atom_code_done,
    atom_code_text,
};

bool atom_code_init (atom_code_t *acode)
{
    atom_init (&acode->atom, ATOM_CODE);
    acode->atom.vmt = &atom_code_vmt;
    return true;
}

atom_code_t *atom_code_new (void)
{
    atom_code_t *acode = malloc (sizeof (atom_code_t));
    if (acode && !atom_code_init (acode))
    {
        free (acode);
        acode = NULL;
    }

    return acode;
}

void atom_code_done (atom_code_t *acode)
{
    atom_done (&acode->atom);
}

void atom_code_free (atom_code_t *acode)
{
    atom_code_done (acode);
    free (acode);
}

static void atom_code_text (atom_t *atom, str_t *str)
{
    (void)atom;
    str_init (str);
}
