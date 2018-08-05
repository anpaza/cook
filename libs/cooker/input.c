/* The Cook project
 * Cook script tokenizer
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "input.h"

#include <string.h>
#include <limits.h>

void input_init (input_t *input)
{
    memset (input, 0, sizeof (input_t));
    str_init (&input->text);
    str_init (&input->name);
    input->stmt_indent = INT_MAX;
    vector_init (&input->stmt_indent_vec, 0);
}

void input_done (input_t *input)
{
    str_done (&input->text);
    str_done (&input->name);
}

void input_set_text (input_t *input, str_t *text, str_t *name)
{
    memset (&input, 0, sizeof (input));

    str_set (&input->text, text);
    str_set (&input->name, name);
}

void input_push_indent (input_t *input, int indent)
{
    vector_append (&input->stmt_indent_vec, (void *)(intptr_t)input->stmt_indent);
    input->stmt_indent = indent;
}

void input_pop_indent (input_t *input)
{
    void *indent = vector_pop (&input->stmt_indent_vec);
    input->stmt_indent = (int)(intptr_t)indent;
}
