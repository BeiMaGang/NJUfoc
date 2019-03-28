%{
    #include "syntax.tab.h"
    #include "syntaxTree.h"
    extern int yycolumn;
    extern YYSTYPE yylval;
%}
%option yylineno

digit   [0-9]
letter  [A-Za-z]
id      (_|{letter})(_|{letter}|{digit})*
INT     [+-]?{digit}+
FLOAT   [+-]?{digit}+(\.{digit}+)?(E[+-]?{digit}+)?
ws      [ \t]

%%
{INT} {yylval.node = createNode("INT",0); return INT;}
{FLOAT} {yylval.node = createNode("FLOAT",0); return FLOAT;}
";" {yylval.node = createNode("SEMI",0); return SEMI;}
"," {yylval.node = createNode("COMMA",0); return COMMA;}
"=" {yylval.node = createNode("ASSIGNOP",0); return ASSIGNOP;}
">" {yylval.node = createNode("RELOP",0); return RELOP;}
"<" {yylval.node = createNode("RELOP",0); return RELOP;}
">=" {yylval.node = createNode("RELOP",0); return RELOP;}
"<=" {yylval.node = createNode("RELOP",0); return RELOP;}
"==" {yylval.node = createNode("RELOP",0); return RELOP;}
"!=" {yylval.node = createNode("RELOP",0); return RELOP;}
"+" {yylval.node = createNode("PLUS",0); return PLUS;}
"-" {yylval.node = createNode("MINUS",0); return MINUS;}
"*" {yylval.node = createNode("STAR",0); return STAR;}
"/" {yylval.node = createNode("DIV",0); return DIV;}
"&&" {yylval.node = createNode("AND",0); return AND;}
"||" {yylval.node = createNode("OR",0); return OR;}
"." {yylval.node = createNode("DOT",0); return DOT;}
"!" {yylval.node = createNode("NOT",0); return NOT;}
"(" {yylval.node = createNode("LP",0); return LP;}
")" {yylval.node = createNode("RP",0); return RP;}
"[" {yylval.node = createNode("LB",0); return LB;}
"]" {yylval.node = createNode("RB",0); return RB;}
"{" {yylval.node = createNode("LC",0); return LC;}
"}" {yylval.node = createNode("RC",0); return RC;}
struct {yylval.node = createNode("STRUCT",0); return STRUCT;}
return {yylval.node = createNode("RETURN",0); return RETURN;}
if {yylval.node = createNode("IF",0); return IF;}
else {yylval.node = createNode("ELSE",0); return ELSE;}
while {yylval.node = createNode("WHILE",0); return WHILE;}
(int)|(float) {yylval.node = createNode("TYPE",0); return TYPE;}
{id} {yylval.node = createNode("ID",0); return ID;}
{ws}+ {}
\n {yycolumn = 1;}

"/*"(.|\n)*"*/" {printf("todo : %s\n", yytext);}
"//".*$ {printf("todo : %s\n", yytext);}

. {printf("Error type A at Line %d: \"%s\"\n", yylineno, yytext);}



%%
/*
int main(int argc, char** argv){
    if(argc < 2){
        yylex();
    }
    else{
        for(int i = 1; i < argc; i++){
            FILE *f = fopen(argv[i],"r");
            if(!f){
                perror(argv[i]);
                return 1;
            }
            YY_BUFFER_STATE bp = yy_create_buffer(f, YY_BUF_SIZE);
            yy_switch_to_buffer(bp);
            yylex();
            yy_delete_buffer(bp);
            fclose(f);
            printf("%s total lines : %d \n\n\n", argv[i], yylineno);
            yylineno = 1;
        }
    }
    return 0;
}
*/

int yywrap(){
    return 1;
}
