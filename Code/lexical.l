%{
    int LINE = 1;

%}
%option yylineno

digit   [0-9]
letter  [A-Za-z]
id      (_|{letter})(_|{letter}|{digit})*


%%
";" {/*SEMI*/}
"," {/*COMMA*/}
"=" {/*ASSIGNOP*/}
">" {/*RELOP*/}
"<" {/*RELOP*/}
">=" {/*RELOP*/}
"<=" {/*RELOP*/}
"==" {/*RELOP*/}
"!=" {/*RELOP*/}
"+" {/*PLUS*/}
"-" {/*MINUS*/}
"*" {/*STAR*/}
"/" {/*DIV*/}
"&&" {/*AND*/}
"||" {/*OR*/}
"." {/*DOT*/}
"!" {/*NOT*/}
int|float {/*TYPE*/}
"(" {/*LP*/}
")" {/*RP*/}
"[" {/*LB*/}
"]" {/*RB*/}
"{" {/*LC*/}
"}" {/*RC*/}
struct {/*STRUCT*/}
return {/*RETURN*/}
if {/*IF*/}
else {/*ELSE*/}
while {/*WHILE*/}
{ws}+ {}
\n {LINE++;}

"/*"[\n|.]*"*/" {printf("todo : %s\n", yytext);}
"//".*$ {printf("todo : %s\n", yytext);}
{id} {}
{num} {}

. {printf("error line %d \"%s\"\n", yylineno, yytext);}



%%
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


int yywrap(){
    return 1;
}
