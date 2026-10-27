%{
#include <stdio.h>
#include <stdlib.h>
#include "register_alloc.h"

int yylex(void);
void yyerror(const char *message);
%}

%union {
    char *str;
}

%token <str> IDENT
%token NUMBER INVALID

%start input

%%

input
    : /* empty */
    | input statement
    ;

statement
    : IDENT '=' { begin_statement($1); free($1); } expression ';'
      { finish_statement(); }
    ;

expression
    : expression '+' term
    | expression '-' term
    | term
    ;

term
    : term '*' factor
    | term '/' factor
    | factor
    ;

factor
    : IDENT  { add_current_use($1); free($1); }
    | NUMBER
    | '(' expression ')'
    ;

%%

void yyerror(const char *message)
{
    fprintf(stderr, "Syntax error: %s\n", message);
    parse_error = 1;
}
