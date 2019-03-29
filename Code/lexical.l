%{
    #include "syntax.tab.h"
    #include "syntaxTree.h"
    extern YYSTYPE yylval;

    int yycolumn = 1;
    #define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
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

\/\*(([^\*]*(\*[^\/])?)*)\*\/ {printf("todo : %s\n", yytext);}
"//".*$ {printf("todo : %s\n", yytext);}

. {printf("Error type A at Line %d: Mysterious character\"%s\"\n", yylineno, yytext);}



%%
int yywrap(){
    return 1;
}
