/* The Cook project
 * The token object API.
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "token.h"

#include <string.h>

static const char *token_names [] =
{
    "error",
    "space",
    "next",
    "comment",
    "word",
    "assign",
    "append",
    "exclude",
    "conditional assign",
    "unveil",
    "simple unveil",
    "open brace",
    "closed brace",
    "comma"
};

void token_init (token_t *token)
{
    memset (token, 0, sizeof (token_t));
}

void token_done (token_t *token)
{
    str_done (&token->text);
}

const char *token_name (token_code_t code)
{
    return token_names [code];
}

void token_copy (token_t *to, token_t *from)
{
    token_done (to);

    str_set (&to->text, &from->text);
    to->code = from->code;
}
