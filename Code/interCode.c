#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "interCode.h"

FILE* f;
void initCode(){
    begin = (InterCode)malloc(sizeof(struct InterCode_));
    memset(begin, 0, sizeof(struct InterCode_));
    begin->pre = begin;
    begin->next = begin;
    begin->kind = CODE_BEGIN;

    defaultPlace = createOperand();
    defaultPlace->kind = VAR;
    defaultPlace->u.value = "wg";
}

void insertCode(InterCode code){
    code->next = begin;
    code->pre = begin->pre;
    begin->pre->next = code;
    begin->pre = code;
}


void printCode(){
    f = fopen("interCode", "w");
    InterCode p = begin->next;
    while(p->kind != CODE_BEGIN){
        switch(p->kind){
            case FUNCTION_C:{
                printf("FUNCTION\n");
                fputs("FUNCTION ", f);
                printOp(p->u.sinop.op);
                fputs(" :\n", f);
                break;
            }
            case PARAM:{
                printf("PARAM\n");
                fputs("PARAM ", f);
                printOp(p->u.sinop.op);
                fputs("\n", f);
                break;
            }
            case ASSIGN:{
                printf("ASSIGN\n");
                printOp(p->u.assign.left);
                fputs(" := ", f);
                printOp(p->u.assign.right);
                fputs("\n", f);
                break;
            }
            case OPERATION:{
                printf("OPERATION\n");
                printOp(p->u.binop.result);
                fputs(" := ", f);
                printOp(p->u.binop.op1);
                if(!strcmp(p->u.binop.sign, "PLUS")){
                    fputs(" + ", f);
                }else if(!strcmp(p->u.binop.sign, "MINUS")){
                    fputs(" - ", f);                    
                }else if(!strcmp(p->u.binop.sign, "STAR")){
                    fputs(" * ", f);                   
                }else if(!strcmp(p->u.binop.sign, "DIV")){
                    fputs(" / ", f);                   
                }
                printOp(p->u.binop.op2);
                fputs("\n", f);
                break;
            }
            case ARG_C: {
                fputs("ARG ",f);
                printOp(p->u.sinop.op);
                fputs("\n", f);
                break;
            }
            case DEC:{
                fputs("DEC ",f);
                printOp(p->u.binop.op1);
                fputs(" ",f);
                printOp(p->u.binop.op2);
                fputs("\n", f);
                break;
            }
        }
        p = p->next;
    }
    printf("print finished\n");
    fclose(f);
}


void printOp(Operand p){
    assert(p != NULL);
    switch(p->kind){
        case VAR:{  
            fputs(p->u.value, f);
            break;
        }
        case CONSTANT_FLOAT:{
            fputs("#", f);
            fprintf(f, "%f", p->u.floatVar);
            break;
        }
        case CONSTANT_INT:{
            fputs("#", f);
            fprintf(f, "%d", p->u.intVar);
            break;
        }
        case _SIZE:{
            fprintf(f, "%d", p->u.intVar);
            break;
        }
        case ADDR2VAR:{
            fputs("*", f);
            fprintf(f, "%s", p->u.value);
            break;
        }
        case VAR2ADDR :{
            fputs("&", f);
            fprintf(f, "%s", p->u.value);
            break;
        }
        case CALL:{
            fputs("CALL ", f);
            fprintf(f, "%s", p->u.value);
            break;
        }
        default: printf("%d\n",p->kind); assert(0);
    }
}

Operand createOperand(){
    Operand t = (Operand)malloc(sizeof(struct Operand_));
    memset(t, 0, sizeof(struct Operand_));
    t->u.value = "null";
    return t;
}

InterCode createInterCode(){
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    memset(code, 0, sizeof(struct InterCode_));
    insertCode(code);
    return code;
}