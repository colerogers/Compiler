%option yylineno
%pointer

%{
    #include <stdlib.h>
    #include <errno.h>
    #include <limits.h>
    #include "ast.hpp"
    #include "parser.hpp"
    void yyerror(const char *);
%}

/* WRITEME: Copy any definitions and start conditions from Project 3 here */

%%

  /* WRITEME: Copy your Flex rules from Project 3 here */

.                 { yyerror("invalid character"); }

%%

int yywrap(void) {
  return 1;
}
