#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "interCode.h"

#define OPTIMIZE

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


void printCode(char* file){
    f = fopen(file, "w");
#ifdef OPTIMIZE
    optimizeGoto();
    deleteLabel();
    optimizeOperate();
#endif
    InterCode p = begin->next;
    while(p->kind != CODE_BEGIN){
        switch(p->kind){
            case FUNCTION_C:{
                // printf("FUNCTION\n");
                fputs("FUNCTION ", f);
                printOp(p->u.sinop.op);
                fputs(" :\n", f);
                break;
            }
            case PARAM:{
                // printf("PARAM\n");
                fputs("PARAM ", f);
                printOp(p->u.sinop.op);
                fputs("\n", f);
                break;
            }
            case ASSIGN:{
                // printf("ASSIGN\n");
                printOp(p->u.assign.left);
                fputs(" := ", f);
                printOp(p->u.assign.right);
                fputs("\n", f);
                break;
            }
            case OPERATION:{
                // printf("OPERATION\n");
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
            case RETURN_C:{
                fputs("RETURN ",f);
                printOp(p->u.sinop.op);
                fputs("\n", f);
                break;
            }
            case IF_GOTO:{
                fputs("IF ",f);
                printOp(p->u.if_goto.t1);
                fputs(" ",f);
                fputs(p->u.if_goto.op,f);
                fputs(" ",f);
                printOp(p->u.if_goto.t2);
                fputs(" GOTO ",f);
                printOp(p->u.if_goto.label);
                fputs("\n", f);
                break;
            }
            case GOTO:{
                fputs("GOTO ",f);
                printOp(p->u.sinop.op);
                fputs("\n", f);
                break;
            }
            case LABEL_C:{
                fputs("LABEL ",f);
                printOp(p->u.sinop.op);
                fputs(" :\n", f);
                break;
            }
            case WRITE:{
                fputs("WRITE ",f);
                printOp(p->u.sinop.op);
                fputs("\n", f);
                break;
            }
            case READ:{
                fputs("READ ",f);
                printOp(p->u.sinop.op);
                fputs("\n", f);
                break;
            }
            default:  printf("%d\n",p->kind); assert(0);
        }
        p = p->next;
    }
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


void optimizeGoto(){
    InterCode interCode;
    for(interCode = begin->next; interCode->kind != CODE_BEGIN; interCode=interCode->next){
        if(interCode->kind == IF_GOTO){
            InterCode interCodo_if_goto = interCode;
            InterCode interCode_goto = interCode->next;

            if(interCode_goto->kind == CODE_BEGIN){
                continue;
            }
            InterCode interCode_label = interCode_goto->next;
            if(interCode_label->kind == CODE_BEGIN){
                continue;
            }

            if(interCode_goto->kind == GOTO && 
                interCode_label->kind == LABEL_C && 
                interCodo_if_goto->u.if_goto.label == interCode_label->u.sinop.op){
                    
                interCodo_if_goto->u.if_goto.label = interCode_goto->u.sinop.op;
                deleteNode(interCode_goto);

                if(!strcmp(interCodo_if_goto->u.if_goto.op,"==")){
                    char* opposite_op = malloc(4);
                    memset(opposite_op,0,4);
                    strcpy(opposite_op,"!=");
                    interCodo_if_goto->u.if_goto.op = opposite_op;
                }else if(!strcmp(interCodo_if_goto->u.if_goto.op,">=")){
                    char* opposite_op = malloc(4);
                    memset(opposite_op,0,4);
                    strcpy(opposite_op,"<");
                    interCodo_if_goto->u.if_goto.op = opposite_op;
                }else if(!strcmp(interCodo_if_goto->u.if_goto.op,"<")){
                    char* opposite_op = malloc(4);
                    memset(opposite_op,0,4);
                    strcpy(opposite_op,">=");
                    interCodo_if_goto->u.if_goto.op = opposite_op;
                }else if(!strcmp(interCodo_if_goto->u.if_goto.op,"!=")){
                    char* opposite_op = malloc(4);
                    memset(opposite_op,0,4);
                    strcpy(opposite_op,"==");
                    interCodo_if_goto->u.if_goto.op = opposite_op;
                }else if(!strcmp(interCodo_if_goto->u.if_goto.op,">")){
                    char* opposite_op = malloc(4);
                    memset(opposite_op,0,4);
                    strcpy(opposite_op,"<=");
                    interCodo_if_goto->u.if_goto.op = opposite_op;
                }else if(!strcmp(interCodo_if_goto->u.if_goto.op,"<=")){
                    char* opposite_op = malloc(4);
                    memset(opposite_op,0,4);
                    strcpy(opposite_op,">");
                    interCodo_if_goto->u.if_goto.op = opposite_op;
                }
            }
        }else if(interCode->kind == GOTO){
            InterCode interCode_goto = interCode;
            InterCode interCode_label = interCode_goto->next;
            if(interCode_label->kind == CODE_BEGIN){
                continue;
            }
            if(interCode_label->kind==LABEL_C && interCode->u.sinop.op==interCode_label->u.sinop.op){
                deleteNode(interCode_goto);
            }
        }
    }
}

void optimizeOperate(){
    while(1){
        int isChanged = 0;
        InterCode code = begin->next;
        while(code->kind != CODE_BEGIN){
            if(code->kind == OPERATION){
                if(isTemp(code->u.binop.result->u.value) &&
                    (code->u.binop.op1->kind == code->u.binop.op2->kind) && 
                (code->u.binop.op1->kind == CONSTANT_INT || code->u.binop.op1->kind == CONSTANT_FLOAT)){
                    isChanged = 1;
                    if(code->u.binop.op1->kind == CONSTANT_INT){
                        code->u.binop.result->kind = CONSTANT_INT;
                        int temp = 0;
                        if(!strcmp(code->u.binop.sign, "PLUS")){
                            temp = code->u.binop.op1->u.intVar + code->u.binop.op2->u.intVar;
                        }else if(!strcmp(code->u.binop.sign, "MINUS")){
                            temp = code->u.binop.op1->u.intVar - code->u.binop.op2->u.intVar;
                        }else if(!strcmp(code->u.binop.sign, "STAR")){
                            temp = code->u.binop.op1->u.intVar * code->u.binop.op2->u.intVar;
                        }else if(!strcmp(code->u.binop.sign, "DIV")){
                            temp = code->u.binop.op1->u.intVar / code->u.binop.op2->u.intVar;
                        }
                        code->u.binop.result->u.intVar = temp;
                    }else{
                        code->u.binop.result->kind = CONSTANT_FLOAT;
                        float temp;
                        if(!strcmp(code->u.binop.sign, "PLUS")){
                            temp = code->u.binop.op1->u.floatVar + code->u.binop.op2->u.floatVar;
                        }else if(!strcmp(code->u.binop.sign, "MINUS")){
                            temp = code->u.binop.op1->u.floatVar - code->u.binop.op2->u.floatVar;
                        }else if(!strcmp(code->u.binop.sign, "STAR")){
                            temp = code->u.binop.op1->u.floatVar * code->u.binop.op2->u.floatVar;
                        }else if(!strcmp(code->u.binop.sign, "DIV")){
                            temp = code->u.binop.op1->u.floatVar / code->u.binop.op2->u.floatVar;
                        }
                        code->u.binop.result->u.floatVar = temp;
                    }
                    InterCode temp = code;
                    code = code->next;
                    deleteNode(temp);
                }else
                    code = code->next;
            }else
                code = code->next;
        }
        if(!isChanged)
            break;
    }
}

void deleteNode(InterCode intercode){
    InterCode temp = intercode;
    intercode->pre->next = intercode->next;
    intercode->next->pre = intercode->pre;
    free(temp);
}


void deleteLabel(){
    InterCode interCode = begin->next;
    while(interCode->kind != CODE_BEGIN){
        if(interCode->kind == LABEL_C){
            InterCode interCode_next = interCode->next;
            if(interCode_next->kind == CODE_BEGIN){
                interCode = interCode->next;
                continue;
            }
            if(interCode_next->kind == LABEL_C){
                Operand operand = interCode->u.sinop.op;
                Operand operand_next = interCode_next->u.sinop.op;

                InterCode temp = interCode;
                interCode = interCode->next;
                deleteNode(temp);
                redirect_label(operand, operand_next);
            }else{
                interCode = interCode->next;
            }
        }else{
            interCode=interCode->next;
        }
    }
}

void redirect_label(Operand operand, Operand operand_next){
    InterCode intercode = begin->next;
    while(intercode->kind != CODE_BEGIN){
        if(intercode->kind == GOTO){
            if(intercode->u.sinop.op == operand){
                intercode->u.sinop.op = operand_next;
            }         
        }else if(intercode->kind == IF_GOTO){
            if(intercode->u.if_goto.label == operand){
                intercode->u.if_goto.label = operand_next;
            }  
        }

        intercode=intercode->next;
    }
}

int isTemp(char* name){
    if(name[0] == 't' && name[1] == 'T')
        return 1;
    return 0;
}