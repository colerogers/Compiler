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

/* print statement */
print {return T_PRINT;}


/* conditional statement */
if {return T_IF;}
else {return T_ELSE;}
while {return T_WHILE;}
do {return T_DO;}


/* data types */
integer {return T_INTEGER;}
boolean {return T_BOOLEAN;}


/* class operators */
new {return T_NEW;}
class {return T_CLASS;}
extends {return T_EXTENDS;}


/* return types */
none {return T_NONE;}


/* comparison */
equals {return T_EQUALS;}
">" {return T_GREATER_THAN;}
">=" {return T_GREATER_THAN_EQUAL;}


/* misc */
"->" {return T_ARROW;}
<<EOF>> {yyterminate();}


/* evaluation operators */
"+" {return T_PLUS;}
"-" {return T_MINUS;}
"*" {return T_MULTIPLICATION;}
"/" {return T_DIVIDE;}
and {return T_AND;}
or {return T_OR;}
not {return T_NOT;}

/* blocks and groups */
"," {return T_COMMA;}
"{" {return T_OPEN_BRACE;}
"}" {return T_CLOSE_BRACE;}
"(" {return T_OPEN_PAREN;}
")" {return T_CLOSE_PAREN;}
";" {return T_SEMI;}
"." {return T_DOT;}
"=" {return T_EQ_SIGN;}


/* comment block */
"/*" {BEGIN(COMMENT);}
<COMMENT>"*/" {BEGIN(INITIAL);}
<COMMENT>\n {}
<COMMENT>. {}
<COMMENT><<EOF>> {yyerror("Invalid EOF, non-terminating Comment");}

/* function tokens */
return {return T_RETURN;}
":" {return T_COLON;}
true {return T_TRUE;}
false {return T_FALSE;}


[A-Za-z][A-Za-z0-9]* {return T_ID;}
[0]|[1-9][0-9]* {return T_LITERAL;}
[ \t\n]+ ;

.                 { yyerror("invalid character"); }

%%

int yywrap(void) {
  return 1;
}
