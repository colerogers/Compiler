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
%precedence T_NOT UMINUS

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
Class : ClassName T_EXTENDS SuperClassName T_OPEN_BRACE ClassBody T_CLOSE_BRACE
| ClassName T_OPEN_BRACE ClassBody T_CLOSE_BRACE
;
ClassBody : Members Methods
| Members
| Methods
|%empty
;
Members : Members Type MemberName T_SEMI
| Type MemberName T_SEMI
;
Methods : Methods MethodDeclaration
| MethodDeclaration
;
MethodDeclaration : MethodName T_OPEN_PAREN Parameters T_CLOSE_PAREN T_ARROW ReturnType T_OPEN_BRACE Body T_CLOSE_BRACE
| MethodName T_OPEN_PAREN T_CLOSE_PAREN T_ARROW ReturnType T_OPEN_BRACE Body T_CLOSE_BRACE
;
Parameters : Parameters T_COMMA Type T_ID
| Type T_ID
;

Body : Declarations Statements Return
| Declarations Return
| Statements Return
| Declarations Statements
| Declarations
| Statements
| Return
|%empty
;

Declarations : Declarations Type Dec T_SEMI
| Type Dec T_SEMI
;
Dec : Dec T_COMMA T_ID
| T_ID
;

Statements : Statements State_Def
| State_Def
;
State_Def : Assignment
| MethodCall T_SEMI
| If_Else
| While_Loop
| Do_While
| Print
;

Return : T_RETURN Expression T_SEMI
;

Assignment : T_ID T_EQ_SIGN Expression T_SEMI
| T_ID T_DOT T_ID T_EQ_SIGN Expression T_SEMI
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

/*
Members : Type MemberName T_SEMI Members
| MethodName T_OPEN_PAREN IsMethod T_CLOSE_BRACE 
| T_CLOSE_BRACE 
;
IsMethod : Parameters T_CLOSE_PAREN T_ARROW ReturnType T_OPEN_BRACE Body T_CLOSE_BRACE MoreMethods
;
MoreMethods: MethodName T_OPEN_PAREN Parameters T_CLOSE_PAREN T_ARROW ReturnType T_OPEN_BRACE Body T_CLOSE_BRACE MoreMethods
|%empty
;
Parameters : Type T_ID 
| Type T_ID T_COMMA Parameters
|%empty
;


Body : Type T_ID T_COMMA Comma
| Type T_ID T_SEMI Body
| T_ID T_EQ_SIGN Assignment  
| T_ID T_DOT T_ID T_EQ_SIGN Assignment
| T_IF If_Else
| T_WHILE While_Loop
| T_DO Do_While
| T_PRINT Print
| T_RETURN Return
|%empty 
;
Comma : T_ID T_SEMI Body 
| T_ID T_COMMA Comma 
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
*/

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
