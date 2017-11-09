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
%x COMMENT

%%

  /* WRITEME: Copy your Flex rules from Project 3 here */
print {return T_PRINT;}
return {return T_RETURN;}
if {return T_IF;}
else {return T_ELSE;}
while {return T_WHILE;}
new {return T_NEW;}
integer {return T_INTEGER;}
boolean {return T_BOOLEAN;}
none {return T_NONE;}
equals {return T_EQUALS;}
and {return T_AND;}
or {return T_OR;}
not {return T_NOT;}
true {return T_TRUE;}
false {return T_FALSE;}
extends {return T_EXTENDS;}
do {return T_DO;}
"=" {return T_EQ_SIGN;}
"+" {return T_PLUS;}
"-" {return T_MINUS;}
"*" {return T_MULTIPLICATION;}
"/" {return T_DIVIDE;}
">" {return T_GREATER_THAN;}
">=" {return T_GREATER_THAN_EQUAL;}
"," {return T_COMMA;}
"{" {return T_OPEN_BRACE;}
"}" {return T_CLOSE_BRACE;}
"(" {return T_OPEN_PAREN;}
")" {return T_CLOSE_PAREN;}
";" {return T_SEMI;}
"->" {return T_ARROW;}
"/*" {BEGIN(COMMENT);}
<COMMENT>"*/" {BEGIN(INITIAL);}
<COMMENT>\n {}
<COMMENT>. {}
<COMMENT><<EOF>> {yyerror("Invalid EOF, non-terminating Comment");}

"." {return T_DOT;}
[A-Za-z][A-Za-z0-9]* {return T_ID;}
[0]|[1-9][0-9]* {return T_LITERAL;}
[ \t\n]+ ;

.                 { yyerror("invalid character"); }

%%

int yywrap(void) {
  return 1;
}
