/* The Cook project
 * Cook script parser
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include "tokenizer.h"
#include "var.h"

typedef struct _parser_t parser_t;

/**
 * A location within input text.
 *
 * It is identic to default YYLTYPE, this allows us to just
 * define YYLTYPE_IS_TRIVIAL and rely on default implementation.
 */
typedef struct
{
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} parser_pos_t;

/**
 * The callback used by the parser to display fatal errors.
 * The function may not return, if desired. If it doesn't, the
 * parser function will return failure.
 *
 * @param parser The parser object contains the source text, file name
 *     and other useful information for displaying error texts.
 * @param pos This field contains the exact error location.
 * @param msg Error description.
 */
typedef void (*parser_error_func_t) (
        parser_t *parser, parser_pos_t *pos, const char *msg);

/**
 * Current parser state.
 */
typedef struct _parser_t
{
    /// Parser input to extract tokens from.
    input_t input;
    /// The root context
    var_t *ctx_root;
    /// Current context (pointer somewhere inside the ctx_root tree)
    var_t *ctx_cur;
    /// The function used to display errors, if not NULL
    parser_error_func_t error;
} parser_t;

/**
 * Initialize a clean parser state object.
 *
 * @param parser The parser state object to initialize
 * @param ctx_root The root context or NULL to use the default root context.
 * @param error The error handler for this context.
 */
extern void parser_init (parser_t *parser, var_t *ctx_root,
                         parser_error_func_t error);

/**
 * Finalize the parser object.
 *
 * @param parser The parser to finalize.
 */
extern void parser_done (parser_t *parser);

/**
 * Parse a expression.
 *
 * @param parser The parser object.
 * @param text The input text.
 * @param name Text identifier for error reporting.
 * @return
 */
extern vector_atom_t *parser_expr (parser_t *parser, str_t *text, str_t *name);

#endif /* __PARSER_H__ */
