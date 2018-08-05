/* The Cook project
 * Atoms containing text.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __atom_text_H__
#define __atom_text_H__

#include "atom.h"

/**
 * A text atom contains a single piece of text.
 */
typedef struct
{
    /// Parent class
    atom_t atom;
    /// The text of the atom
    str_t text;
} atom_text_t;

/**
 * Intialize a text atom object.
 *
 * @param atext The text atom to initialize
 * @param text The text to assign to the atom.
 * @return false on memory allocation problems.
 */
extern bool atom_text_init (atom_text_t *atext, str_t *text);

/**
 * Create a new text atom object.
 *
 * @param text The text to assign to the atom.
 * @return The new object or NULL on memory allocation problems.
 */
extern atom_text_t *atom_text_new (str_t *text);

/**
 * Terminate a text atom.
 *
 * @param atext The atom to terminate.
 */
extern void atom_text_done (atom_text_t *atext);

/**
 * Terminate and free a text atom.
 *
 * @param atext The atom to terminate and free.
 */
extern void atom_text_free (atom_text_t *atext);

#endif /* __atom_text_H__ */
