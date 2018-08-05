/* The Cook project
 * Cook script tokenizer
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include "input.h"
#include "token.h"

/**
 * Extract the next token from input stream
 *
 * @param input The input text.
 * @param token The structure is filled with the next token.
 * It is assumed initially in non-initialized state.
 * @return true if next token is available, false on EOF
 */
extern bool input_token (input_t *input, token_t *token);

#endif /* __TOKENIZER_H__ */
