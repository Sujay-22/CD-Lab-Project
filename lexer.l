%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
%}

ID      [a-zA-Z_][a-zA-Z0-9_]*
NUMBER  [0-9]+([.][0-9]+)?

%%
[ \t\r\n]+        ;
{ID}               {
                       yylval.str = (char *)malloc((size_t)yyleng + 1);
                       if (yylval.str == NULL) {
                           fprintf(stderr, "Out of memory while reading identifier.\n");
                           exit(EXIT_FAILURE);
                       }
                       memcpy(yylval.str, yytext, (size_t)yyleng);
                       yylval.str[yyleng] = '\0';
                       return IDENT;
                    }
{NUMBER}           { return NUMBER; }
[=+\-*/();]        { return (unsigned char)yytext[0]; }
.                  {
                       fprintf(stderr, "Invalid character: '%s'\n", yytext);
                       return INVALID;
                    }
%%

int yywrap(void)
{
    return 1;
}
