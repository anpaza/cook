/* The Cook project
 * Cook script tokenizer
 *
 * Copyright (c) 2018 Andrey Zabolotnyi <zapparello@ya.ru>
 * See file docs/COPYING for copying conditions
 */

#include "tokenizer.h"

#include <string.h>
#include <assert.h>
#include <limits.h>

// must be power of two
static const int cook_tab_spaces = 8;
static const char cook_stop_characters [] = " \t\r\n#,{}=$";
static const char cook_allowed_escapes [] = "abtnre{}$\"s#.,=+-?\\ux";

static bool cook_spaces (input_t *input, token_t *token)
{
    token->line = input->line;
    token->column = input->column;
    int start_ofs = input->ofs;

    token->code = TOK_SPACE;

    while (input->ofs < input->text.size)
    {
        switch (input->text.data [input->ofs])
        {
            case ' ':
                input->column++;
                break;

            case '\t':
                input->column = (input->column + cook_tab_spaces)
                        & ~(cook_tab_spaces - 1);
                break;

            case '\r':
                break;

            case '\n':
                input->line++;
                input->column = 0;
                break;

            default:
                // not a space
                goto done;
        }

        input->ofs++;
    }

done:
    // If spaces span across newline
    if (token->line != input->line)
    {
        input->indent = input->column;

        // and end before the next_indent, it's a NEXT
        if (input->indent <= input->stmt_indent)
            token->code = TOK_NEXT;
    }

    int length = input->ofs - start_ofs;
    if (length == 0)
        return false;

    str_init_c_const (&token->text, input->text.data + start_ofs, length);

    return true;
}

static void cook_comment (input_t *input, token_t *token)
{
    int start_ofs = input->ofs;
    input->ofs++;

    str_find_char (&input->text, &input->ofs, '\n');

    // we don't care if we found \n or not...
    int comment_size = input->ofs - start_ofs;
    input->column += comment_size;

    str_init_c_const (&token->text, input->text.data + start_ofs, comment_size);
    token->code = TOK_COMMENT;
}

// Got a single-character operator
static void cook_operator_1 (input_t *input, token_t *token, token_code_t code)
{
    str_init_c_const (&token->text, input->text.data + input->ofs, 1);
    token->code = code;

    input->ofs++;
    input->column++;
}

// Two-character operator
static bool cook_operator_2 (input_t *input, token_t *token,
                             char second, token_code_t code)
{
    int ofs = input->ofs + 1;
    if (ofs >= input->text.size)
        return false;

    if (input->text.data [ofs] != second)
        return false;

    str_init_c_const (&token->text, input->text.data + input->ofs, 2);
    token->code = code;

    input->ofs += 2;
    input->column += 2;

    return true;
}

static void token_append_char (token_t *token, input_t *input, int *pos)
{
    const char *cptr = input->text.data + *pos;
    str_append_c_const (&token->text, cptr, 1);
    if (*cptr == '\n')
    {
        input->line++;
        input->column = 0;
    }
    else
        input->column++;

    (*pos)++;
}

static bool token_append_entity (
        token_t *token, input_t *input, int *pos, const char *allowed_chars)
{
    while (true)
    {
        token_append_char (token, input, pos);
        if (*pos >= input->text.size)
            return false;

        char c = input->text.data [*pos];

        // ';' ends entity
        if (c == ';')
            return true;

        if (!strchr (allowed_chars, c))
            return false;
    }
}

bool input_token (input_t *input, token_t *token)
{
    token_init (token);

    // Skip initial spaces
    if (cook_spaces (input, token))
        return true;

    if (input->ofs >= input->text.size)
        return false;

    switch (input->text.data [input->ofs])
    {
        case '#':
            cook_comment (input, token);
            return true;

        case '=':
            cook_operator_1 (input, token, TOK_ASSIGN);
            return true;

        case '?':
            if (cook_operator_2 (input, token, '=', TOK_COND_ASSIGN))
                return true;
            break;

        case '+':
            if (cook_operator_2 (input, token, '=', TOK_APPEND))
                return true;
            break;

        case '-':
            if (cook_operator_2 (input, token, '=', TOK_EXCLUDE))
                return true;
            break;

        case '$':
            if (cook_operator_2 (input, token, '{', TOK_UNVEIL))
                return true;
            cook_operator_1 (input, token, TOK_SIMPLE_UNVEIL);
            return true;

        case '{':
            cook_operator_1 (input, token, TOK_BRACE_OPEN);
            return true;

        case '}':
            cook_operator_1 (input, token, TOK_BRACE_CLOSE);
            return true;

        case ',':
            cook_operator_1 (input, token, TOK_COMMA);
            return true;
    }

    // otherwise it's a word...
    str_init (&token->text);
    token->code = TOK_WORD;

    int ofs = input->ofs;
    bool dquotes = false;

    while (ofs < input->text.size)
    {
        char c = input->text.data [ofs];

        if (dquotes)
        {
            if (c == '"')
            {
                ofs++;
                input->column++;

                if ((ofs < input->text.size) &&
                    (input->text.data [ofs] == '"'))
                {
                    // That's a double double quote :)
                }
                else
                {
                    // That's the finishing double quote
                    dquotes = false;
                    continue;
                }
            }
        }
        else if (c == '\\')
        {
            // Place the backslash into the output word.
            // Unquoting happens later, when we use the word.
            token_append_char (token, input, &ofs);

            if (ofs >= input->text.size)
                goto error;

            c = input->text.data [ofs];
            const char *escape = strchr (cook_allowed_escapes, c);

            // Invalid escape, that's a fatal error
            if (!escape)
                goto error;

            // unicode decimal escape \u[0-9]+;
            // or hexadecimal escape \x[0-9a-fA-F]+;
            if (((c == 'u') &&
                 !token_append_entity (token, input, &ofs, "0123456789")) ||
                ((c == 'x') &&
                 !token_append_entity (token, input, &ofs, "0123456789ABCDEFabcdef")))
                goto error;

            // just append the quote code to output word...
        }
        else if (c == '"')
        {
            dquotes = true;
            ofs++;
            input->column++;
            continue;
        }
        else if (strchr (cook_stop_characters, c))
        {
            // A stop character cannot come first in a word
            assert (token->text.size != 0);

            // Break word parsing
            break;
        }
        // += -= ?=
        else if (((c == '+') || (c == '-') || (c == '?')) &&
                 (ofs + 1 < input->text.size))
        {
            // Check if next character is a '='
            c = input->text.data [ofs + 1];
            if (c == '=')
            {
                // A stop character cannot come first in a word
                assert (token->text.size != 0);
                
                // Break word parsing
                break;
            }
        }

        token_append_char (token, input, &ofs);
    }

    if (!dquotes)
    {
        input->ofs = ofs;
        return true;
    }

    // Unterminated ", that's a fatal error

error:
    str_done (&token->text);
    token->code = TOK_ERROR;
    input->ofs = ofs;
    return true;
}
