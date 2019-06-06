#include <stdio.h>
#include "type.h"
#include "semantic.h"
#include "targetCode.h"


extern int  yyparse();
extern FILE *yyin;
extern int yylineno;
extern int wrong;
int main(int argc, char** argv){
    // if(argc < 2){
    //     yyparse();
    // }
    // else{
        // for(int i = 0; i < argc - 1; i++){
        for(int i=1; i<argc; i++){
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
    // }
    if(!wrong){
        initialType();
        initSymbolTable();
        initCode();
        Program();
        // printCode(argv[argc - 1]);
        printCode("result_test.txt");
        printAllTargetCode("result.s");
    }
    return 0;
}