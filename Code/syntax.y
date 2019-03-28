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

    #define YYERROR_VERBOSE
    int wrong = 0;
%}
/*test*/
%union{
    struct TreeNode* node;
}


%token <node> PLUS MINUS STAR DIV
%token <node> SEMI
%token <node> COMMA
%token <node> ASSIGNOP
%token <node> RELOP
%token <node> AND OR
%token <node> DOT
%token <node> NOT
%token <node> TYPE
%token <node> LP RP LB RB LC RC
%token <node> STRUCT
%token <node> RETURN
%token <node> IF
%token <node> ELSE
%token <node> WHILE
%token <node> INT
%token <node> FLOAT
%token <node> ID
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StrucuSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

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
Program : ExtDefList {$$ = createNode("Program",1,$1);if(!wrong) prePrint($$,1);}
        ;
ExtDefList : ExtDef ExtDefList {$$ = createNode("ExtDecList",2,$1,$2);}
        |{ $$ = createNode("ExtDefList",0);setFlag($$);}
        ;
ExtDef  : Specifier ExtDecList SEMI {$$ = createNode("ExtDef",3,$1,$2,$3);}
        | Specifier SEMI {$$ = createNode("ExtDef",2,$1,$2);}
        | Specifier FunDec CompSt {$$ = createNode("ExtDef",3,$1,$2,$3);}
        ;
ExtDecList : VarDec {$$ = createNode("ExtDecList",1,$1);}
        | VarDec COMMA ExtDecList {$$ = createNode("ExtDecList",3,$1,$2,$3);}
        ;

Specifier : TYPE {$$ = createNode("Specifier",1,$1);}
        | StrucuSpecifier {$$ = createNode("Specifier",1,$1);}
        ;
StrucuSpecifier : STRUCT OptTag LC DefList RC {$$ = createNode("StrucuSpecifier",5,$1,$2,$3,$4,$5);}
        | STRUCT Tag {$$ = createNode("StrucuSpecifier",2,$1,$2);}
        ;
OptTag  : ID {$$ = createNode("OptTag",1,$1);}
        |{ $$ = createNode("OptTag",0);setFlag($$);}
        ;
Tag     : ID {$$ = createNode("Tag",1,$1);}
        ;

VarDec  : ID {$$ = createNode("VarDec",1,$1);}
        | VarDec LB INT RB {$$ = createNode("VarDec",4,$1,$2,$3,$4);}
        ;
FunDec  : ID LP VarList RP {$$ = createNode("FunDec",4,$1,$2,$3,$4);}
        | ID LP RP {$$ = createNode("FunDec",3,$1,$2,$3);}
        ;
VarList : ParamDec COMMA VarList {$$ = createNode("VarList",3,$1,$2,$3);}
        | ParamDec {$$ = createNode("VarList",1,$1);}
        ;
ParamDec : Specifier VarDec {$$ = createNode("ParamDec",2,$1,$2);}
        ;

CompSt  : LC DefList StmtList RC {$$ = createNode("CompSt",4,$1,$2,$3,$4);}
        ;
StmtList: Stmt StmtList {$$ = createNode("StmtList",2,$1,$2);}
        |{ $$ = createNode("StmtList",0);setFlag($$);}
        ;
Stmt    : Exp SEMI {$$ = createNode("Stmt",2,$1,$2);}
        | CompSt {$$ = createNode("Stmt",1,$1);}
        | RETURN Exp SEMI {$$ = createNode("Stmt",3,$1,$2,$3);}
        | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = createNode("Stmt",5,$1,$2,$3,$4,$5);}
        | IF LP Exp RP Stmt ELSE Stmt {$$ = createNode("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
        | WHILE LP Exp RP Stmt {$$ = createNode("Stmt",5,$1,$2,$3,$4,$5);}
        | error SEMI {yyerror("nihao");}
        ;

DefList : Def DefList {$$ = createNode("DefList",2,$1,$2);}
        |{ $$ = createNode("DefList",0);setFlag($$);}
        ;
Def     : Specifier DecList SEMI {$$ = createNode("Def",3,$1,$2,$3);}
        ;
DecList : Dec {$$ = createNode("DecList",1,$1);}
        | Dec COMMA DecList {$$ = createNode("DecList",3,$1,$2,$3);}
        ;
Dec     : VarDec {$$ = createNode("Dec",1,$1);}
        | VarDec ASSIGNOP Exp {$$ = createNode("Dec",3,$1,$2,$3);}
        ;

Exp     : Exp ASSIGNOP Exp {$$ = createNode("Exp",3,$1,$2,$3);}
        | Exp AND Exp {$$ = createNode("Exp",3,$1,$2,$3);}
        | Exp OR Exp {$$ = createNode("Exp",3,$1,$2,$3);}
        | Exp RELOP Exp {$$ = createNode("Exp",3,$1,$2,$3);}
        | Exp PLUS Exp {$$ = createNode("Exp",3,$1,$2,$3);}
        | Exp MINUS Exp {$$ = createNode("Exp",3,$1,$2,$3);}
        | Exp STAR Exp {$$ = createNode("Exp",3,$1,$2,$3);}
        | Exp DIV Exp {$$ = createNode("Exp",3,$1,$2,$3);}
        | LP Exp RP {$$ = createNode("Exp",3,$1,$2,$3);}
        | MINUS Exp {$$ = createNode("Exp",2,$1,$2);}
        | NOT Exp {$$ = createNode("Exp",2,$1,$2);}
        | ID LP Args RP {$$ = createNode("Exp",4,$1,$2,$3,$4);}
        | ID LP RP {$$ = createNode("Exp",3,$1,$2,$3);}
        | Exp LB Exp RB {$$ = createNode("Exp",4,$1,$2,$3,$4);}
        | Exp DOT ID {$$ = createNode("Exp",3,$1,$2,$3);}
        | ID {$$ = createNode("Exp",1,$1);}
        | INT {$$ = createNode("Exp",1,$1);}
        | FLOAT {$$ = createNode("Exp",1,$1);}
        ;
Args    : Exp COMMA Args {$$ = createNode("Args",3,$1,$2,$3);}
        | Exp {$$ = createNode("Args",1,$1);}
        ;
%%

void yyerror(char* msg){
    wrong = 1;
    fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}