/* The Cook project
 * The token object API.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __TOKEN_H__
#define __TOKEN_H__

#include "str.h"

/**
 * Token codes
 */
typedef enum
{
    /// tokenizer error
    TOK_ERROR,
    /// Whitespace
    TOK_SPACE,
    /// The special "next" whitespace token
    TOK_NEXT,
    /// # ...
    TOK_COMMENT,
    /// A word
    TOK_WORD,
    /// =
    TOK_ASSIGN,
    /// +=
    TOK_APPEND,
    /// -=
    TOK_EXCLUDE,
    /// ?=
    TOK_COND_ASSIGN,
    /// ${
    TOK_UNVEIL,
    /// $
    TOK_SIMPLE_UNVEIL,
    /// {
    TOK_BRACE_OPEN,
    /// }
    TOK_BRACE_CLOSE,
    /// ,
    TOK_COMMA,
} token_code_t;

/**
 * A single token from the input file.
 * The 'text' field usually uses copy-on-write semantics,
 * so it takes up little memory.
 */
typedef struct
{
    /// The text of the token
    str_t text;
    /// Token code
    token_code_t code;
    /// The starting line of the token (copied from input->line)
    int line;
    /// The starting column of the token (copied from input->column)
    int column;
} token_t;

/**
 * Initialize the token object.
 * @param token The token to initialize.
 */
extern void token_init (token_t *token);

/**
 * Free the contents of a token object.
 * @param token The token to finish.
 */
extern void token_done (token_t *token);

/**
 * Get the user-friendly name of the token
 * @param code
 * @return
 */
extern const char *token_name (token_code_t code);

/**
 * Copy the token.
 * @param to The token to copy TO. The token must be initialized.
 * @param from The token to copy FROM.
 */
extern void token_copy (token_t *to, token_t *from);

#endif /* __TOKEN_H__ */
