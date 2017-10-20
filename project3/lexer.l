%option yylineno
%pointer

%{
    #include <cstdlib>
    #include <cerrno>
    #include <climits>
    #include <limits>
    #include "parser.hpp"

enum Token {
T_PRINT,
T_RETURN,
T_IF,
T_ELSE,
T_WHILE,
T_NEW,
T_INTEGER,
T_BOOLEAN,
T_NONE,
T_EQUALS,
T_AND,
T_OR,
T_NOT,
T_TRUE,
T_FALSE,
T_EXTENDS,
T_DO,
T_PLUS,
T_MINUS,
T_MULTIPLICATION,
T_DIVIDE,
T_GREATER_THAN,
T_GREATER_THAN_EQUAL,
T_COMMA,
T_OPEN_BRACE,
T_CLOSE_BRACE,
T_OPEN_PAREN,
T_CLOSE_PAREN,
T_SEMI,
T_COMMENT_START,
T_COMMENT_END,
T_NEWLINE,
T_SPACE,
T_LITERAL,
T_ID
};
    
	void yyerror(const char *);
%}

/* WRITEME: Write any definitions here. You can find information on
            definitions here:
http://flex.sourceforge.net/manual/Definitions-Section.html */

/* WRITEME: If you are going to use start conditions, write them here.
            You can find more information on start conditions here:
            http://flex.sourceforge.net/manual/Start-Conditions.html
            You will probably want to use exclusive conditions rather
            than inclusive conditions (use the %x instead of the %s). */

%%

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
"+" {return T_PLUS;}
"-" {return T_MINUS;}
"*" {return T_MULTIPLICATION;}
"/" {return T_DIVIDE;}
">" {return T_GREATER_THAN;}
">=" {return T_GREATER_THAN_EQUAL;}
"," {return T_COMMA;}
{ {return T_OPEN_BRACE;}
} {return T_CLOSE_BRACE;}
"(" {return T_OPEN_PAREN;}
")" {return T_CLOSE_PAREN;}
; {return T_SEMI;}
"/*" {return T_COMMENT_START;}
"*/" {return T_COMMENT_END;}
\\n {return T_NEWLINE;}
[A-Za-z][A-Za-z0-9]* {return T_ID;}
[0]|[1-9][0-9]* {return T_LITERAL;}

.                 { yyerror("invalid character"); }

%%

int yywrap(void) {
  return 1;
}
