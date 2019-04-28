#include <stdio.h>
#include "type.h"
#include "semantic.h"

extern int  yyparse();
extern FILE *yyin;
extern int yylineno;
extern int wrong;
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
            yylineno = 1;
        }
    }
    if(!wrong){
        initialType();
        Program();
    }
    return 0;
}
