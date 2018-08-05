/* The Cook project
 * Cook script parser
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "parser.h"

#include <stdlib.h>
#include <assert.h>

void parser_init (parser_t *parser, var_t *ctx_root, parser_error_func_t error)
{
    assert (parser);

    input_init (&parser->input);
    if (!ctx_root)
        ctx_root = var_get_root_ctx ();

    parser->ctx_root = parser->ctx_cur = ctx_root;
    parser->error = error;
}

void parser_done (parser_t *parser)
{
    assert (parser);

    input_done (&parser->input);
}

#if 0
bool parser_recipe (parser_t *parser, str_t *text, str_t *name)
{
    parser_error_t errcode;

    input_set_text (&parser->input, text, name);
}

vector_word_t *parser_ ()
{
    token_t token;
    // The part on the left of the assignment operator
    vector_word_t lval;
    vector_word_init (&lval);

    // adjoin words/lists if true
    bool adjoin = false;

    /* read tokens until input ends */
    while (input_token (&parser->input, &token))
    {
        switch (token.code)
        {
            case TOK_ERROR:
                errcode = ERR_LEXICAL;
                goto error;

            case TOK_SPACE:
                // ignore white spaces
            case TOK_COMMENT:
                // ignore comments
                adjoin = false;
                break;

            case TOK_WORD:
            {
                word_t *new_word = word_new (&token.text, false);
                if (adjoin)
                {
                    vector_word_adjoin_word (&lval, new_word);
                    word_free (new_word);
                }
                else
                {
                    vector_append (&lval, new_word);
                    adjoin = true;
                }
                break;
            }

            case TOK_ASSIGN:
            case TOK_APPEND:
            case TOK_COND_ASSIGN:
                adjoin = false;
                break;

            case TOK_EXPAND:
            case TOK_SIMPLE_EXPAND:
            {
                vector_word_t result;
                vector_word_init (&result);

                if (!parser_expression (parser, &token, &result))
                {
                    errcode = ERR_EXPAND;
                    goto error;
                }

                if (adjoin)
                    vector_word_adjoin (&lval, &result);
                else
                    vector_join (&lval, &result);

                vector_done (&result);
                break;
            }

            case TOK_BRACE_OPEN:
            case TOK_BRACE_CLOSE:
            case TOK_COMMA:
                errcode = ERR_UNEXPECTED;
                goto error;
        }

        // done with this token, go on with the next one
        token_done (&token);

        // If we got words, this is a implicit expansion.
        if (lval.size > 1)
        {
            if (!parser_expression (parser, &token, &lval))
            {
                errcode = ERR_EXPAND;
                goto error;
            }
        }
    }

    /* input consumed, success */
    return true;

error:
    /* all error situations jump here */
    if (parser->error)
        parser->error (errcode, parser);

    token_done (&token);
    return false;
}
#endif
