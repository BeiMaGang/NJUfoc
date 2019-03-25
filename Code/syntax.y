%locations
%{
    #include <stdio.h>
    #include "lex.yy.c"
    int yycolumn = 1;
    #define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}
/*test*/
%union{
    struct TreeNode* node;
}

%token PLUS MINUS STAR DIV
%token SEMI
%token COMMA
%token ASSIGNOP
%token RELOP
%token AND OR
%token DOT
%token NOT
%token TYPE
%token LP RP LB RB LC RC
%token STRUCT
%token RETURN
%token IF
%token ELSE
%token WHILE
%token INT
%token FLOAT
%token ID

/*associativity and priorities*/
%right ASSIGNOP
%left  OR
%left  AND
%left  RELOP
%left  PLUS MINUS
%left  STAR DIV
/*qu fu hao mei you xie*/
%right NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program : ExtDefList
        ;
ExtDefList : ExtDef ExtDefList
        |
        ;
ExtDef  : Specifier ExtDecList SEMI
        | Specifier SEMI
        | Specifier FunDec CompSt
        ;
ExtDecList : VarDec
        | VarDec COMMA ExtDecList
        ;

Specifier : TYPE 
        | StrucuSpecifier
        ;
StrucuSpecifier : STRUCT OptTag LC DefList RC
        | STRUCT Tag
        ;
OptTag  : ID
        |
        ;
Tag     : ID
        ;

VarDec  : ID
        | VarDec LB INT RB
        ;
FunDec  : ID LP VarList RP
        | ID LP RP
        ;
VarList : ParamDec COMMA VarList
        | ParamDec
        ;
ParamDec : Specifier VarDec
        ;

CompSt  : LC DefList StmtList RC
        ;
StmtList: Stmt StmtList
        |
        ;
Stmt    : Exp SEMI
        | CompSt
        | RETURN Exp SEMI
        | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
        | IF LP Exp RP Stmt ELSE Stmt
        | WHILE LP Exp RP Stmt
        ;

DefList : Def DefList
        |
        ;
Def     : Specifier DecList SEMI
        ;
DecList : Dec
        | Dec COMMA DecList
        ;
Dec     : VarDec
        | VarDec ASSIGNOP Exp
        ;

Exp     : Exp ASSIGNOP Exp
        | Exp AND Exp
        | Exp OR Exp
        | Exp RELOP Exp
        | Exp PLUS Exp
        | Exp MINUS Exp
        | Exp STAR Exp
        | Exp DIV Exp
        | LP Exp RP
        | MINUS Exp
        | NOT Exp
        | ID LP Args RP
        | ID LP RP
        | Exp LB Exp RB
        | Exp DOT ID
        | ID
        | INT
        | FLOAT
        ;
Args    : Exp COMMA Args
        | Exp
        ;
%%

int yyerror(char* msg){
    fprintf(stderr, "%d line error: %s\n", yylloc.first_line, msg);
}