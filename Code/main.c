#include <stdio.h>

extern int  yyparse();
extern FILE *yyin;
extern int yylineno;
int main(int argc, char** argv){
    if(argc < 2){
        yyparse();
    }
    else{
        for(int i = 1; i < argc; i++){
            FILE *f = fopen(argv[i],"r");
            if(!f){
                perror(argv[i]);
                return 1;
            }
            yyin = f;
            yyparse();
            fclose(f);
            printf("%s total lines : %d \n\n\n", argv[i], yylineno);
            yylineno = 1;
        }
    }
    return 0;
}
