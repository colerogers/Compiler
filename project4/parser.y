%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <iostream>

    #include "ast.hpp"
    
    #define YYDEBUG 1
    int yylex(void);
    void yyerror(const char *);
    
    extern ASTNode* astRoot;
%}

%error-verbose

/* WRITEME: Copy your token and precedence specifiers from Project 3 here */
%token <print_ptr> T_PRINT
%token <returnstatement_ptr> T_RETURN
%token <ifelse_ptr> T_IF
%token T_ELSE
%token <while_ptr> T_WHILE
%token <new_ptr> T_NEW
%token <integertype_ptr> T_INTEGER
%token <booleanliteral_ptr> T_BOOLEAN
%token <none_ptr> T_NONE
%token <equal_ptr> T_EQUALS
%token <and_ptr> T_AND
%token <or_ptr> T_OR
%token <not_ptr> T_NOT
%token <integer_ptr> T_TRUE
%token <integer_ptr> T_FALSE
%token T_EXTENDS
%token <dowhile_ptr> T_DO
%token <plus_ptr> T_PLUS
%token <minus_ptr> T_MINUS
%token <times_ptr> T_MULTIPLICATION
%token <divide_ptr> T_DIVIDE
%token <greater_ptr> T_GREATER_THAN
%token <greaterequal_ptr> T_GREATER_THAN_EQUAL
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
%token <integer_ptr> T_LITERAL
%token <identifier_ptr> T_ID
%token T_DOT
%token <assignment_ptr> T_EQ_SIGN
%token T_ARROW
%token T_CLASS
%token T_COLON

%left T_OR
%left T_AND
%left T_GREATER_THAN T_GREATER_THAN_EQUAL T_EQUALS
%left T_PLUS T_MINUS
%left T_MULTIPLICATION T_DIVIDE
%precedence T_NOT UMINUS

/* WRITEME: Specify types for all nonterminals and necessary terminals here */
 /*%type <program_ptr> Start
%type <class_ptr> Class
%type <class_list_ptr> Language
%type <declaration_ptr> Members
%type <methodbody_ptr> IsMethod*/
%type <returnstatement_ptr> Return
%type 

%type <expression_ptr> Expression MethodCall
%type <expression_list_ptr> Arguments ArgumentsPrime
%type <type_ptr> Type ReturnType 
%type <identifier_ptr> ClassName SuperClassName MethodName MemberName

%%

/* WRITEME: This rule is a placeholder. Replace it with your grammar
            rules from Project 3 */

Start : Language  { /* Empty action */ }
      ;
Language : Class LanguagePrime
;
LanguagePrime : Language
|%empty
;
Class : ClassName ClassPrime
;
ClassPrime : T_EXTENDS SuperClassName T_OPEN_BRACE Members 
| T_OPEN_BRACE Members
;
Members : Type MemberName T_SEMI Members
| MethodName T_OPEN_PAREN IsMethod T_CLOSE_BRACE /* CLASS CLOSE */
| T_CLOSE_BRACE /* CLASS CLOSE */
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

Return : Expression T_SEMI                                    { $$ = new ReturnStatementNode($1); }
;
State_Def : T_ID T_EQ_SIGN Assignment                         { $$ = new AssignmentNode($1, NULL, $3); }
| T_ID T_DOT T_ID T_EQ_SIGN Assignment                        { $$ = new AssignmentNode($1, $3, $5); }
| T_IF If_Else                                                { $$ = $2; }
| T_WHILE While_Loop                                          { $$ = $2; }
| T_DO Do_While                                               { $$ = $2; }
| T_PRINT Print                                               { $$ = $2; }
| T_RETURN Return                                             { $$ = $2; }
|%empty                                                       { $$ = NULL; }
;
Assignment : Expression T_SEMI State_Def                      { $$ = $1; }
;
If_Else : Expression T_OPEN_BRACE Block T_CLOSE_BRACE State_Def { $$ = new IfElseNode($1, $3, NULL); }
| Expression T_OPEN_BRACE Block T_CLOSE_BRACE T_ELSE Else     { $$ = new IfElseNode($1, $3, $6); }
;
Else : T_OPEN_BRACE Block T_CLOSE_BRACE State_Def             { $$ = $2; }
;
While_Loop : Expression T_OPEN_BRACE Block T_CLOSE_BRACE State_Def { $$ = new WhileNode($1, $3); }
;
Do_While : T_OPEN_BRACE Block T_CLOSE_BRACE T_WHILE T_OPEN_PAREN Expression T_CLOSE_PAREN T_SEMI State_Def { $$ = new DoWhileNode($2, $6); }
;
Print : Expression T_SEMI State_Def                            { $$ = new PrintNode($1); }
;
Block : T_ID T_EQ_SIGN Assignment                              { $$ = new AssignmentNode($1, NULL, $3); }
| T_ID T_DOT T_ID T_EQ_SIGN Assignment                         { $$ = new AssignmentNode($1, $3, $5); }
| T_IF If_Else                                                 { $$ = $2; }
| T_WHILE While_Loop                                           { $$ = $2; }
| T_DO Do_While                                                { $$ = $2; }
| T_PRINT Print                                                { $$ = $2; }
| T_RETURN Return                                              { $$ = $2; }
;

Expression : Expression T_PLUS Expression                      { $$ = new PlusNode($1, $3); }
| Expression T_MINUS Expression %prec T_MINUS                  { $$ = new MinusNode($1, $3); }
| Expression T_MULTIPLICATION Expression %prec T_MULTIPLICATION { $$ = new TimesNode($1, $3); }
| Expression T_DIVIDE Expression %prec T_DIVIDE                { $$ = new DivideNode($1, $3); }
| Expression T_GREATER_THAN Expression %prec T_GREATER_THAN    { $$ = new GreaterNode($1, $3); }
| Expression T_GREATER_THAN_EQUAL Expression %prec T_GREATER_THAN_EQUAL { $$ = new GreaterEqualNode($1, $3); }
| Expression T_EQUALS Expression %prec T_EQUALS                { $$ = new EqualNode($1, $3); }
| Expression T_AND Expression %prec T_AND                      { $$ = new AndNode($1, $3); }
| Expression T_OR Expression %prec T_OR                        { $$ = new OrNode($1, $3); }
| T_NOT Expression %prec T_NOT                                 { $$ = new NotNode($2); }
| T_MINUS Expression %prec UMINUS                              { $$ = new NegationNode($2); }
| T_ID /*{std::cout<<"using ID in exp"<<std::endl;}*/          { $$ = new VariableNode($1); }
| T_ID T_DOT T_ID                                              { $$ = new MemberAccessNode($1, $3); }
| MethodCall                                                   { $$ = $1; }
| T_OPEN_PAREN Expression T_CLOSE_PAREN                        { $$ = $2; }
| T_LITERAL                                                    { $$ = new IntegerLiteralNode($1); }
| T_TRUE                                                       { $$ = new BooleanLiteralNode($1); }
| T_FALSE                                                      { $$ = new BooleanLiteralNode($1); }
| T_NEW ClassName                                              { $$ = new NewNode($2, NULL); }
| T_NEW ClassName T_OPEN_PAREN Arguments T_CLOSE_PAREN         { $$ = new NewNode($2, $4); }
;
MethodCall : T_ID T_OPEN_PAREN Arguments T_CLOSE_PAREN         { $$ = new MethodCallNode($1, NULL, $3); }
| T_ID T_DOT T_ID T_OPEN_PAREN Arguments T_CLOSE_PAREN         { $$ = new MethodCallNode($1, $3, $5); }
;
Arguments : ArgumentsPrime                                      { $$ = $1; }
|%empty                                                         { $$ = NULL; }
;
ArgumentsPrime : ArgumentsPrime T_COMMA Expression              { $$ = $1; $$->push_back($3); }
| Expression                                                    { $$ = new std::list<ExpressionNode*>(); $$->push_back($1); }
;

Type : T_INTEGER                                                { $$ = new IntegerTypeNode(); }
| T_BOOLEAN                                                     { $$ = new BooleanTypeNode(); }
| ClassName                                                     { $$ = new ObjectTypeNode($1); }
;
ReturnType : Type                                               { $$ = $1; }
| T_NONE                                                        { $$ = new NoneNode(); }
;
ClassName : T_ID                                                { $$ = $1; }
;
SuperClassName : T_ID                                           { $$ = $1; }
;
MethodName : T_ID                                               { $$ = $1; }
;
MemberName : T_ID                                               { $$ = $1; }
;

%%

extern int yylineno;

void yyerror(const char *s) {
  fprintf(stderr, "%s at line %d\n", s, yylineno);
  exit(0);
}