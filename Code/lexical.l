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
{INT} {yylval.node = createNode("int",0);return INT;}
{FLOAT} {printf("num float %s\n", yytext); return FLOAT;}
";" {printf("%s\n", yytext); return SEMI;}
"," {return COMMA;}
"=" {return ASSIGNOP;}
">" {return RELOP;}
"<" {return RELOP;}
">=" {return RELOP;}
"<=" {return RELOP;}
"==" {return RELOP;}
"!=" {return RELOP;}
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return STAR;}
"/" {return DIV;}
"&&" {return AND;}
"||" {return OR;}
"." {return DOT;}
"!" {return NOT;}
"(" {printf("%s\n", yytext);return LP;}
")" {printf("%s\n", yytext);return RP;}
"[" {printf("%s\n", yytext);return LB;}
"]" {printf("%s\n", yytext);return RB;}
"{" {printf("%s\n", yytext);return LC;}
"}" {printf("%s\n", yytext);return RC;}
struct {return STRUCT;}
return {return RETURN;}
if {return IF;}
else {return ELSE;}
while {return WHILE;}
(int)|(float) {printf("type int\n");return TYPE;}
{id} {printf("id %s\n", yytext); return ID;}
{ws}+ {}
\n {yycolumn = 1;}

"/*"(.|\n)*"*/" {printf("todo : %s\n", yytext);}
"//".*$ {printf("todo : %s\n", yytext);}

. {printf("lexical error line %d \"%s\"\n", yylineno, yytext);}



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
