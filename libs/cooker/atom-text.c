/* The Cook project
 * Atoms containing text.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "atom-text.h"

#include <stdlib.h>

static void atom_text_text (atom_t *atom, str_t *str);

static atom_vmt_t atom_text_vmt =
{
    (void (*) (atom_t *atom))atom_text_done,
    atom_text_text,
};

bool atom_text_init (atom_text_t *atext, str_t *text)
{
    atom_init (&atext->atom, ATOM_TEXT);
    atext->atom.vmt = &atom_text_vmt;
    return str_set (&atext->text, text);
}

atom_text_t *atom_text_new (str_t *text)
{
    atom_text_t *atext = malloc (sizeof (atom_text_t));
    if (atext && !atom_text_init (atext, text))
    {
        free (atext);
        atext = NULL;
    }

    return atext;
}

void atom_text_done (atom_text_t *atext)
{
    atom_done (&atext->atom);
    str_done (&atext->text);
}

void atom_text_free (atom_text_t *atext)
{
    atom_text_done (atext);
    free (atext);
}

void atom_text_text (atom_t *atom, str_t *str)
{
    atom_text_t *atext = (atom_text_t *)atom;
    str_set (str, &atext->text);
}
