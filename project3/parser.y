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
%right UMINUS

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
A : T_CLOSE_BRACE /* CLASS CLOSE */
| Members
;
Members : Type MemberName T_SEMI
| Type MemberName T_SEMI Members
| MethodName T_OPEN_PAREN IsMethod T_CLOSE_BRACE /* CLASS CLOSE */
| T_CLOSE_BRACE /* CLASS CLOSE */
;
IsMethod :  T_CLOSE_PAREN T_ARROW ReturnType T_OPEN_BRACE Methods
| Parameters T_CLOSE_PAREN T_ARROW ReturnType T_OPEN_BRACE Methods
;
Methods : M
| Body M
;
M : T_CLOSE_BRACE MethodName T_OPEN_PAREN IsMethod
| T_CLOSE_BRACE 
;
Parameters : Type T_ID 
| Type T_ID T_COMMA Parameters
;

/*because all expressions in Body can return epsilon, all combinations must be recorded*/
Body : Type T_ID T_COMMA Comma
| Type T_ID T_SEMI Body
| T_ID T_EQ_SIGN Assignment  /*{std::cout<<"hi";}*/
| T_ID T_DOT T_ID T_EQ_SIGN Assignment
| T_IF If_Else
| T_WHILE While_Loop
| T_DO Do_While
| T_PRINT Print
| T_RETURN Return
|%empty /*{std::cout<<"EMPTY";}*/
;
Comma : T_ID T_SEMI Body /*{std::cout<<"T_ID T_SEMI Body"<<std::endl;}*/
| T_ID T_COMMA Comma /*{std::cout<<"T_ID T_COMMA Comma"<<std::endl;}*/
;

Return : Expression T_SEMI
;
State_Def : T_ID T_EQ_SIGN Assignment
| T_ID T_DOT T_ID T_EQ_SIGN Assignment
| T_IF If_Else
| T_WHILE While_Loop
| T_DO Do_While
| T_PRINT Print
| T_RETURN Return
|%empty
;
Assignment : Expression T_SEMI State_Def
;

If_Else : Expression T_OPEN_BRACE Block T_CLOSE_BRACE State_Def
| Expression T_OPEN_BRACE Block T_CLOSE_BRACE T_ELSE Else
;
Else : T_OPEN_BRACE Block T_CLOSE_BRACE State_Def
;
While_Loop : Expression T_OPEN_BRACE Block T_CLOSE_BRACE State_Def
;
Do_While : T_OPEN_BRACE Block T_CLOSE_BRACE T_WHILE T_OPEN_PAREN Expression T_CLOSE_PAREN T_SEMI State_Def
;
Print : Expression T_SEMI State_Def
;

Block : T_ID T_EQ_SIGN Assignment
| T_ID T_DOT T_ID T_EQ_SIGN Assignment
| T_IF If_Else
| T_WHILE While_Loop
| T_DO Do_While
| T_PRINT Print
| T_RETURN Return
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
| T_MINUS Expression %prec UMINUS
| T_ID /*{std::cout<<"using ID in exp"<<std::endl;}*/
| T_ID T_DOT T_ID
| T_ID T_OPEN_PAREN MethodCall
| T_ID T_DOT T_ID T_OPEN_PAREN MethodCall
| T_OPEN_PAREN Expression T_CLOSE_PAREN
| T_LITERAL
| T_TRUE
| T_FALSE
| T_NEW ClassName
| T_NEW ClassName T_OPEN_PAREN Arguments T_CLOSE_PAREN
;
MethodCall : T_CLOSE_PAREN
| Arguments T_CLOSE_PAREN
;
Arguments : ArgumentsPrime
|%empty
;
ArgumentsPrime : ArgumentsPrime T_COMMA Expression
| Expression
;

Type : T_INTEGER
| T_BOOLEAN
| T_ID
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
