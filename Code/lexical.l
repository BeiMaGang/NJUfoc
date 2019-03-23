%{
    #include "syntax.tab.h"
    int LINE = 1;

%}
%option yylineno

digit   [0-9]
letter  [A-Za-z]
id      (_|{letter})(_|{letter}|{digit})*
int     [+-]?{digit}+
float   [+1]?{digit}+(\.{digit}+)?(E[+-]?{digit}+)?
ws      [ \t]

%%
{int} {return INT;}
{float} {return FLOAT;}
";" {return SEMI;}
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
"(" {return LP;}
")" {return RP;}
"[" {return LB;}
"]" {return RB;}
"{" {return LC;}
"}" {return RC;}
struct {return STRUCT;}
return {return RETURN;}
if {return IF;}
else {return ELSE;}
while {return WHILE;}
{id} {return ID;}
{ws}+ {}
\n {LINE++;}

"/*"[\n|.]*"*/" {printf("todo : %s\n", yytext);}
"//".*$ {printf("todo : %s\n", yytext);}

. {printf("error line %d \"%s\"\n", yylineno, yytext);}



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
