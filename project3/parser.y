%{
    #include <cstdlib>
    #include <cstdio>
    #include <iostream>

    #define YYDEBUG 1

    int yylex(void);
    void yyerror(const char *);
%}

%error-verbose

/* WRITEME: List all your tokens here */
%token T_PRINT
%token T_RETURN
%token T_IF
%token T_ELSE
%token T_WHILE
%token T_NEW
%token T_INTEGER
%token T_BOOLEAN
%token T_NONE
%token T_EQUALS
%token T_AND
%token T_OR
%token T_NOT
%token T_TRUE
%token T_FALSE
%token T_EXTENDS
%token T_DO
%token T_PLUS
%token T_MINUS
%token T_MULTIPLICATION
%token T_DIVIDE
%token T_GREATER_THAN
%token T_GREATER_THAN_EQUAL
%token T_COMMA
%token T_OPEN_BRACE
%token T_CLOSE_BRACE
%token T_OPEN_PAREN
%token T_CLOSE_PAREN
%token T_SEMI
%token T_COMMENT_START
%token T_COMMENT_END
%token T_NEWLINE
%token T_SPACE
%token T_LITERAL
%token T_ID
%token T_DOT
%token T_EQ_SIGN



/* WRITEME: Specify precedence here */
%left T_OR
%left T_AND
%left T_GREATER_THAN T_GREATER_THAN_EQUAL T_EQUALS
%left T_PLUS T_MINUS
%left T_MULTIPLICATION T_DIVIDE
%right T_NOT

%%

/* WRITEME: This rule is a placeholder, since Bison requires
            at least one rule to run successfully. Replace
            this with your appropriate start rules. */
Start : Language
      ;

/* WRITME: Write your Bison grammar specification here */
Language : Class LanguagePrime
         ;
LanguagePrime : Language
              |%empty
              ;
Class : ClassName ClassPrime
      ;
ClassPrime : T_EXTENDS SuperClassName T_OPEN_BRACE A
           | T_OPEN_BRACE A
           ;
A : T_CLOSE_BRACE
| Members T_CLOSE_BRACE
| Methods T_CLOSE_BRACE
| Members Methods T_CLOSE_BRACE
;
Members : Declaration Members
|%empty
        ;
Declaration : Type MemberName T_SEMI
;

Methods : MethodName T_OPEN_PAREN Parameters T_CLOSE_PAREN T_MINUS T_GREATER_THAN ReturnType T_OPEN_BRACE MethodsPrime
|%empty
        ;
MethodsPrime : T_CLOSE_BRACE Methods
| Body T_CLOSE_BRACE Methods
Parameters : Param
           |%empty
           ;
Param : Type T_ID ParamPrime
      ;
ParamPrime : T_COMMA Param
|%empty
;
Body : Decs Statements Return
;
Decs : Dec_Def Decs
|%empty
;
Dec_Def : Type T_ID Dec_DefPrime
;
Dec_DefPrime : T_SEMI
| T_COMMA Dec_DefPrimePrime
;
Dec_DefPrimePrime : T_ID Dec_DefPrime
;

Statements : State_Def Statements
|%empty
;
Return : T_RETURN Expression T_SEMI
|%empty
;
State_Def : Assignment
| MethodCall
| Expression
| If_Else
| While_Loop
| Do_While
| Print
;
Assignment : T_ID AssignmentPrime
;
AssignmentPrime : T_EQ_SIGN Expression
| T_DOT T_ID T_EQ_SIGN Expression T_SEMI
;
If_Else : T_IF Expression T_OPEN_BRACE Statements T_CLOSE_BRACE Else
;
Else : T_ELSE T_OPEN_BRACE Statements T_CLOSE_BRACE
|%empty
;
While_Loop : T_WHILE Expression T_OPEN_BRACE Statements T_CLOSE_BRACE
;
Do_While : T_DO T_OPEN_BRACE Statements T_CLOSE_BRACE T_WHILE T_OPEN_PAREN Expression T_CLOSE_PAREN T_SEMI
;
Print : T_PRINT Expression T_SEMI
;

Expression : Expression T_PLUS Expression
| Expression T_MINUS Expression
| Expression T_MULTIPLICATION Expression
| Expression T_DIVIDE Expression
| Expression T_GREATER_THAN Expression
| Expression T_GREATER_THAN_EQUAL Expression
| Expression T_EQUALS Expression
| Expression T_AND Expression
| Expression T_OR Expression
| T_NOT Expression
| T_MINUS Expression %prec T_NOT
| T_ID
| T_ID T_DOT T_ID
| MethodCall
| T_OPEN_PAREN Expression T_CLOSE_PAREN
| T_LITERAL
| T_TRUE
| T_FALSE
| T_NEW ClassName
| T_NEW ClassName T_OPEN_PAREN Arguments T_CLOSE_PAREN
;
MethodCall : T_ID T_OPEN_PAREN Arguments T_CLOSE_PAREN
| T_ID T_DOT T_ID T_OPEN_PAREN Arguments T_CLOSE_PAREN
;
Arguments : ArgumentsPrime
|%empty
;
ArgumentsPrime : ArgumentsPrime T_COMMA Expression
| Expression
;

Type : T_INTEGER
| T_BOOLEAN
| ClassName
;
ReturnType : Type
| T_NONE
;
ClassName : T_ID
;
SuperClassName : ClassName
;
MethodName : T_ID
;
MemberName : T_ID
;
%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(1);
}
