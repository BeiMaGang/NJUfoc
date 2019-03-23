%{
    #include <stdio.h>
    #include "lex.yy.c"
%}

%token PLUS MINUS STAR DIV
%token SEMI
%token COMMA
%token ASSIGNOP
%token RELOP
%token AND OR
%token DOT
%token NOT
%token LP RP LB RB LC RC
%token STRUCT
%token RETURN
%token IF
%token ELSE
%token WHILE
%token INT
%token FLOAT
%token ID

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
TYPE    : INT 
        | FLOAT
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
        | IF LP Exp RP Stmt
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
int main(){
    yyparse();
}

int yyerror(char* msg){
    fprintf(stderr, "error: %s\n", msg);
}