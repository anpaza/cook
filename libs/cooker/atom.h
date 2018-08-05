/* The Cook project
 * The Cook variables.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __atom_H__
#define __atom_H__

#include "str.h"
#include "vector.h"

/// Atom types
typedef enum
{
    /// A text atom
    ATOM_TEXT,
    /// An executable atom
    ATOM_CODE,
} atom_type_t;

typedef struct _atom_t atom_t;

/// Atom virtual method table
typedef struct
{
    /// Free memory associated with this object
    void (*done) (atom_t *atom);

    /// Convert this atom to its text equivalent
    void (*text) (atom_t *atom, str_t *str);
} atom_vmt_t;

/**
 * The basic object which is part of Cook list values.
 * This is an abstract class.
 */
typedef struct _atom_t
{
    /// Virtual method table
    const atom_vmt_t *vmt;

    /// Atom type
    atom_type_t type : 1;

    /// true if this atom has to be adjoined with next
    bool adjoin : 1;
} atom_t;

/**
 * Intialize a atom object.
 *
 * @param atom The atom to initialize
 * @param type Atom type
 */
extern void atom_init (atom_t *atom, atom_type_t type);

/**
 * Allocate a new atom object and initialize it.
 *
 * @param type Atom type
 * @return The new atom or NULL on memory allocation failure.
 */
extern atom_t *atom_new (atom_type_t type);

/**
 * Create a independent copy of the atom.
 *
 * @param atom The atom to make a copy of.
 * @return The new atom or NULL on memory allocation problems.
 */
extern atom_t *atom_new_copy (atom_t *atom);

/**
 * Finalize the atom.
 *
 * @param atom The atom to finalize.
 */
extern void atom_done (atom_t *atom);

/**
 * Finalize the atom and free object memory.
 *
 * @param atom The atom to free.
 */
extern void atom_free (atom_t *atom);

// ---------- // ---------- // ---------- // ---------- // ---------- //

/// A vector of atom_t's
typedef vector_t vector_atom_t;

/**
 * Initialize a vector of atom_t objects. Vector elements
 * will automatically destroy when removed from the vector.
 * The vector is not sortable as there's no compare criteria.
 *
 * @param veca The vector to initialize.
 */
extern void vector_atom_init (vector_atom_t *veca);

#endif /* __atom_H */
