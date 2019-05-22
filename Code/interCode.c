#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include "interCode.h"


void initCode(){
    begin = (InterCode)malloc(sizeof(struct InterCode_));
    memset(begin, 0, sizeof(struct InterCode_));
    begin->pre = begin;
    begin->next = begin;
    begin->kind = CODE_BEGIN;
}

void insertCode(InterCode code){
    code->next = begin;
    code->pre = begin->pre;
    begin->pre->next = code;
    begin->pre = code;
}


void printCode(){
    InterCode p = begin->next;
    while(p->kind != CODE_BEGIN){
        switch(p->kind){
            case FUNCTION_C:{
                printf("FUNCTION : ");
                printf(p->u.sinop.op->u.value);
                printf("\n");
                break;
            }
        }
        p = p->next;
    }
}


void printOp(Operand operand){

}

Operand createOperand(){
    Operand t = (Operand)malloc(sizeof(struct Operand_));
    memset(t, 0, sizeof(struct Operand_));
    return t;
}

InterCode createInterCode(){
    InterCode code = (InterCode)malloc(sizeof(struct InterCode_));
    memset(code, 0, sizeof(struct InterCode_));
    insertCode(code);
    return code;
}