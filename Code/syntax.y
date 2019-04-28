%{
    #include <stdio.h>
    #include "lex.yy.c"

    #define YYERROR_VERBOSE
    int wrong = 0;
    extern FILE *yyin;
    extern struct TreeNode* root;
%}
%locations
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
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

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
Program : ExtDefList {$$ = createNode("Program",1,$1); root = $$; /*if(!wrong) prePrint($$,0);*/}
        ;
ExtDefList : ExtDef ExtDefList {$$ = createNode("ExtDefList",2,$1,$2);}
        |{ $$ = createNode("ExtDefList",0);setFlag($$);}
        ;
ExtDef  : Specifier ExtDecList SEMI {$$ = createNode("ExtDef",3,$1,$2,$3);}
        | Specifier SEMI {$$ = createNode("ExtDef",2,$1,$2);}
        | Specifier FunDec CompSt {$$ = createNode("ExtDef",3,$1,$2,$3);}
        
        | Specifier error{errorinf(@1.last_line, "expecting ;");}
        | Specifier ExtDecList error{errorinf(@2.last_line, "expecting ;");}
        ;
ExtDecList : VarDec {$$ = createNode("ExtDecList",1,$1);}
        | VarDec COMMA ExtDecList {$$ = createNode("ExtDecList",3,$1,$2,$3);}

        | VarDec error ExtDecList {errorinf(@1.last_line, "missing ,");}
        ;

Specifier : TYPE {$$ = createNode("Specifier",1,$1);}
        | StructSpecifier {$$ = createNode("Specifier",1,$1);}
        ;
StructSpecifier : STRUCT OptTag LC DefList RC {$$ = createNode("StructSpecifier",5,$1,$2,$3,$4,$5);}
        | STRUCT Tag {$$ = createNode("StructSpecifier",2,$1,$2);}

        | STRUCT OptTag error DefList RC {errorinf(@2.last_line, "struct expecting (");}
        | STRUCT OptTag LC DefList error {errorinf(@4.last_line, "struct expecting )");}
        ;
OptTag  : ID {$$ = createNode("OptTag",1,$1);}
        |{ $$ = createNode("OptTag",0);setFlag($$);}
        ;
Tag     : ID {$$ = createNode("Tag",1,$1);}
        ;

VarDec  : ID {$$ = createNode("VarDec",1,$1);}
        | VarDec LB INT RB {$$ = createNode("VarDec",4,$1,$2,$3,$4);}
        
        | VarDec error INT RB {errorinf(@1.last_line, "array reference missing [");}
        | VarDec LB INT error {errorinf(@3.last_line, "array reference missing ]");}
        ;
FunDec  : ID LP VarList RP {$$ = createNode("FunDec",4,$1,$2,$3,$4);}
        | ID LP RP {$$ = createNode("FunDec",3,$1,$2,$3);}

        | ID error VarList RP {errorinf(@1.last_line, "expecting (");}
        | ID LP VarList error {errorinf(@3.last_line, "expecting )");}
        | ID error RP {errorinf(@1.last_line, "expecting (");}
        | ID LP error {errorinf(@2.last_line, "expecting )");}
        ;
VarList : ParamDec COMMA VarList {$$ = createNode("VarList",3,$1,$2,$3);}
        | ParamDec {$$ = createNode("VarList",1,$1);}

        | ParamDec error VarList {errorinf(@1.last_line, "ParamDec error");}
        ;
ParamDec : Specifier VarDec {$$ = createNode("ParamDec",2,$1,$2);}

        | error VarDec {errorinf(@2.first_line, "ParamDec error");}
        ;

CompSt  : LC DefList StmtList RC {$$ = createNode("CompSt",4,$1,$2,$3,$4);}
        
        | error DefList StmtList RC {errorinf(@2.first_line, "expecting {");}
        | LC DefList StmtList error {errorinf(@3.last_line, "expecting }");}
        | error RC {errorinf(@2.first_line, "before } expecting ; or ,");}
        ;
        
StmtList: Stmt StmtList {$$ = createNode("StmtList",2,$1,$2);}
        |{ $$ = createNode("StmtList",0);setFlag($$);}

        | error StmtList {errorinf(@2.first_line, "Invalid code block ");}
        ;
Stmt    : Exp SEMI {$$ = createNode("Stmt",2,$1,$2);}
        | CompSt {$$ = createNode("Stmt",1,$1);}
        | RETURN Exp SEMI {$$ = createNode("Stmt",3,$1,$2,$3);}
        | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$ = createNode("Stmt",5,$1,$2,$3,$4,$5);}
        | IF LP Exp RP Stmt ELSE Stmt {$$ = createNode("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
        | WHILE LP Exp RP Stmt {$$ = createNode("Stmt",5,$1,$2,$3,$4,$5);}

        | Exp error {errorinf(@1.last_line, "expecting ;");}
        | RETURN Exp error {errorinf(@2.last_line, "expecting ;");}
        | IF error Exp RP Stmt ELSE Stmt {errorinf(@1.last_line, "expecting (");}
        | IF error Exp error Stmt ELSE Stmt {errorinf(@1.last_line, "expecting ()");}
        | IF LP Exp error Stmt ELSE Stmt {errorinf(@3.last_line, "expecting )");}
        | IF error Exp RP Stmt {errorinf(@1.last_line, "expecting (");}
        | IF LP Exp error Stmt {errorinf(@3.last_line, "expecting )");}
        | WHILE LP Exp error Stmt {errorinf(@3.last_line, "expecting )");}
        | WHILE error Exp RP Stmt {errorinf(@1.last_line, "expecting (");}
        | WHILE LP Exp RP error {errorinf(@4.last_line, "while block error");}
        ;

DefList : Def DefList {$$ = createNode("DefList",2,$1,$2);}
        |{ $$ = createNode("DefList",0);setFlag($$);}
        ;
Def     : Specifier DecList SEMI {$$ = createNode("Def",3,$1,$2,$3);}
        
        | Specifier DecList error {errorinf(@2.last_line, "expecting ;");}
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

        | Exp LB error RB {errorinf(@2.last_line, "array reference");}
        | ID error Args RP {errorinf(@1.last_line, "expecting (");}
        | ID LP Args error {errorinf(@3.last_line, "expecting )");}
        | ID error RP {errorinf(@1.last_line, "expecting (");}
        | ID LP error {errorinf(@2.last_line, "expecting )");}
        ;
Args    : Exp COMMA Args {$$ = createNode("Args",3,$1,$2,$3);}
        | Exp {$$ = createNode("Args",1,$1);}

        | Exp error Args {errorinf(@1.last_line, "expecting ,");}
        ;
%%

void yyerror(char* msg){
    wrong = 1;
    printf("at Line %d: \"%s\"  %s\n", yylineno, msg, yytext);
}

void errorinf(int line, char* content){
//     printf("Error type B at Line %d: \"%s\"\n", line, content);
}