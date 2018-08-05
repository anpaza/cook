%define api.pure full

%{

#include "parser.h"
#include "tokenizer.h"

#include <string.h>

#define YYLTYPE parser_pos_t
#define YYLTYPE_IS_TRIVIAL 1

%}

/* Use symbol locations for error reporting */
%locations

/* Additional yylex() parameter */
%lex-param      { parser_t *parser }
/* Additional yyparse() parameter */
%parse-param    { parser_t *parser }

%union
{
    /* terminal symbol value */
    token_t token;
}

%destructor { token_done (&$$); } <token>

%token <token> SPACE
%token <token> NEXT
%token <token> WORD
%token <token> ASSIGN
%token <token> APPEND
%token <token> EXCLUDE
%token <token> COND_ASSIGN
%token <token> UNVEIL
%token <token> SIMPLE_UNVEIL
%token <token> BRACE_OPEN
%token <token> BRACE_CLOSE
%token <token> COMMA

%{

int yylex (YYSTYPE *sym, YYLTYPE *loc, parser_t *parser);
void yyerror (YYLTYPE *loc, parser_t *parser, const char *msg);

%}

%%

statements:
    %empty
  | assignment statements
  | implicit-unveil statements
  | error NEXT statements
;

assignment:
    identifier-list assign opt-space opt-list NEXT
;

implicit-unveil:
    identifier SPACE args NEXT
;

identifier:
    adjoined-value
;

identifier-list:
    identifier opt-space
  | identifier opt-space COMMA opt-space identifier-list
;

opt-space:
    %empty
  | SPACE
;

assign:
    ASSIGN | APPEND | EXCLUDE | COND_ASSIGN
;

opt-list:
    %empty
  | list
;

list:
    adjoined-value opt-space
  | adjoined-value SPACE list
;

adjoined-value:
    value
  | value adjoined-value
;

value:
    WORD
  | BRACE_OPEN statements BRACE_CLOSE
  | explicit-unveil
;

explicit-unveil:
    SIMPLE_UNVEIL WORD
  | SIMPLE_UNVEIL explicit-unveil
  | UNVEIL opt-space identifier SPACE args BRACE_CLOSE
;

args:
    list
  | list COMMA args
;

%%

int yylex (YYSTYPE *sym, YYLTYPE *loc, parser_t *parser)
{
next:
    loc->first_line = parser->input.line;
    loc->first_column = parser->input.column;

    bool ok = input_token (&parser->input, &sym->token);

    loc->last_line = parser->input.line;
    loc->last_column = parser->input.column;

    if (!ok)
    {
        /* Parser error, display error and return %empty */
        return YYEMPTY;
    }

    /* Take action depending on token type */
    switch (sym->token.code)
    {
#define LT(t)   case TOK_##t: return t;
        LT (SPACE)
        LT (NEXT)
        LT (WORD)
        LT (ASSIGN)
        LT (APPEND)
        LT (EXCLUDE)
        LT (COND_ASSIGN)
        LT (UNVEIL)
        LT (SIMPLE_UNVEIL)
        LT (BRACE_OPEN)
        LT (BRACE_CLOSE)
        LT (COMMA)
#undef LT

        case TOK_ERROR:
            // lexer error, display error and return %empty
            if (parser->error)
                parser->error (parser, loc, YY_(""));
            return YYEMPTY;

        case TOK_COMMENT:
            // Silently ignore comments
            goto next;
    }

    /* should never get here */
    return YYEMPTY;
}

void yyerror (YYLTYPE *loc, parser_t *parser, const char *msg)
{
    if (parser->error)
        parser->error ();
}
