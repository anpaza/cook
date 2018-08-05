/* The Cook project
 * Cook script tokenizer
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __INPUT_H__
#define __INPUT_H__

#include "str.h"
#include "vector.h"

/**
 * Tokenizer input.
 *
 * The pos member can be saved and restored as needed, but it has to
 * refer to same text (obviously).
 */
typedef struct
{
    /// The whole script text
    str_t text;
    /// Current (linear) offset within text
    int ofs;
    /// Current line number (starting from 0)
    int line;
    /// Current column number (starting from 0)
    int column;
    /// Current indent level (number of spaces before 1st token in line)
    int indent;
    /// The indent of next statement (MAX_INT unless set by parser)
    int stmt_indent;
    /// The stack for statement indent values (used for nested statements)
    vector_t stmt_indent_vec;
    /// Identifier (user-comprehensible name, used in error messages)
    str_t name;
} input_t;

/**
 * Initialize the input object.
 *
 * @param input The input object to initialize.
 */
extern void input_init (input_t *input);

/**
 * Finalize the input object.
 *
 * @param input The input object that is finalized.
 */
extern void input_done (input_t *input);

/**
 * Set the input to consume new text.
 *
 * @param input The input object to set up.
 * @param text The text to consume.
 * @param name Text identifier (file, function name, etc).
 */
extern void input_set_text (input_t *input, str_t *text, str_t *name);

/**
 * Push current statement indent into a stack and set current statement
 * indent to @a indent.
 *

 * @param indent The new statement indent.
 */
extern void input_push_indent (input_t *input, int indent);

/**
 * Pop statement indent from the stack.
 *
 * @param input The input object to operate on.
 */
extern void input_pop_indent (input_t *input);

#endif /* __INPUT_H__ */
