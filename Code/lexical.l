%{
    int LINE = 1;

%}
%option yylineno

digit   [0-9]
letter  [A-Za-z]
id      (_|{letter})(_|{letter}|{digit})*

%%
if |
else |
for |
while |
printf |
scanf {printf("key words \n");}
{id} {printf("id : %s \n", yytext);}
{num} {printf("a num : %s\n", yytext);}
\n {LINE++;}
[ \t] {}


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
