/* The Cook project
 * Atoms containing virtual processor code.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __atom_code_H__
#define __atom_code_H__

#include "atom.h"

/**
 * A code atom contains a series of instructions for the virtual
 * processor that results in a value_t.
 */
typedef struct
{
    /// Parent class
    atom_t atom;
} atom_code_t;


/**
 * Intialize a code atom object.
 *
 * @param acode The code atom to initialize
 * @return false on memory allocation problems.
 */
extern bool atom_code_init (atom_code_t *acode);

/**
 * Create a new code atom object.
 *
 * @return The new object or NULL on memory allocation problems.
 */
extern atom_code_t *atom_code_new (void);

/**
 * Terminate a code atom.
 *
 * @param acode The atom to terminate.
 */
extern void atom_code_done (atom_code_t *acode);

/**
 * Terminate and free a code atom.
 *
 * @param acode The atom to terminate and free.
 */
extern void atom_code_free (atom_code_t *acode);

#endif /* __atom_code_H__ */
