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
%token T_ARROW



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
Language : Class | Class Language
;
Class : ClassName ClassPrime
;
ClassPrime : T_EXTENDS SuperClassName T_OPEN_BRACE A
| T_OPEN_BRACE A
;
A : T_CLOSE_BRACE
| Members
;
Members : Type MemberName T_SEMI
| Type MemberName T_SEMI Members
| MethodName T_OPEN_PAREN IsMethod
;
IsMethod :  T_CLOSE_PAREN T_ARROW ReturnType T_OPEN_BRACE Methods
| Parameters T_CLOSE_PAREN T_ARROW ReturnType T_OPEN_BRACE Methods
;
Methods : M
| Body M
;
M : MethodName T_OPEN_PAREN IsMethod
| T_CLOSE_BRACE
;
Parameters : Type T_ID 
| Type T_ID T_COMMA Parameters
;

/*because all expressions in Body can return epsilon, all combinations must be recorded*/
Body : Type T_ID CommaOrSemi
| T_ID T_EQ_SIGN Assignment
| T_ID T_DOT T_ID T_EQ_SIGN Assignment
| T_IF If_Else
| T_WHILE While_Loop
| T_DO Do_While
| T_PRINT Print
| Expression State_Def
| T_RETURN Return
| T_CLOSE_BRACE
;
CommaOrSemi : T_SEMI Body
| T_COMMA T_ID CommaOrSemi
;


/*
Body : Decs Statements Return
| Decs Statements
| Decs Return
| Decs
| Statements Return
| Statements
| Return
;
Decs : Dec_Def Decs
| Dec_Def
;
Dec_Def : Type T_ID Dec_DefPrime
;
Dec_DefPrime : T_SEMI
| T_COMMA Dec_DefPrimePrime
;
Dec_DefPrimePrime : T_ID Dec_DefPrime
;


Statements : State_Def Statements
| State_Def
;
*/
Return : T_RETURN Expression T_SEMI
;
State_Def : T_ID Assignment
| T_ID T_DOT T_ID Assignment
| T_IF If_Else
| T_WHILE While_Loop
| T_DO Do_While
| T_PRINT Print
| Expression State_Def
| T_RETURN Return
| T_CLOSE_BRACE
;
Assignment : Expression T_SEMI State_Def
;

If_Else : Expression T_OPEN_BRACE State_Def T_CLOSE_BRACE
| Expression T_OPEN_BRACE State_Def T_CLOSE_BRACE T_ELSE Else
;
Else : T_OPEN_BRACE State_Def T_CLOSE_BRACE State_Def
;
While_Loop : Expression T_OPEN_BRACE State_Def T_CLOSE_BRACE State_Def
;
Do_While : T_OPEN_BRACE State_Def T_CLOSE_BRACE T_WHILE T_OPEN_PAREN Expression T_CLOSE_PAREN T_SEMI State_Def
;
Print : Expression T_SEMI State_Def
;

Expression : Expression T_PLUS Expression
| Expression T_MINUS Expression %prec T_MINUS
| Expression T_MULTIPLICATION Expression %prec T_MULTIPLICATION
| Expression T_DIVIDE Expression %prec T_DIVIDE
| Expression T_GREATER_THAN Expression %prec T_GREATER_THAN
| Expression T_GREATER_THAN_EQUAL Expression %prec T_GREATER_THAN_EQUAL
| Expression T_EQUALS Expression %prec T_EQUALS
| Expression T_AND Expression %prec T_AND
| Expression T_OR Expression %prec T_OR
| T_NOT Expression %prec T_NOT
| T_MINUS Expression %prec T_NOT
| T_ID
| T_ID T_DOT T_ID
| T_ID T_OPEN_PAREN MethodCall
| T_ID T_DOT T_ID T_OPEN_PAREN MethodCall
| T_OPEN_PAREN Expression T_CLOSE_PAREN
| T_INTEGER T_LITERAL
| T_TRUE
| T_FALSE
| T_NEW ClassName
| T_NEW ClassName T_OPEN_PAREN Arguments T_CLOSE_PAREN
;
MethodCall : T_CLOSE_PAREN
| Arguments T_CLOSE_PAREN
;
Arguments  : Expression ArgumentsPrime
;
ArgumentsPrime : T_COMMA ArgumentsPrime
|%empty
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
