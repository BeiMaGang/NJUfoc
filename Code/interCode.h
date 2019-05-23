#ifndef INTER_CODE_H
#define INTER_CODE_H
#include <stdio.h>
#include <memory.h>

typedef struct Operand_* Operand;
struct Operand_ {
    enum{DEFAULT, VAR, CONSTANT_INT, CONSTANT_FLOAT, VAR2ADDR, ADDR2VAR, LABLE, CALL, _SIZE}kind;
    union{
        int intVar;
        float floatVar;
        char* value;
    }u;
};

typedef struct InterCode_* InterCode; 
struct InterCode_ {
    enum {
            CODE_BEGIN, 
            ASSIGN, OPERATION, 
            GOTO, IF_GOTO, 
            FUNCTION_C, RETURN_C, ARG_C, 
            DEC, PARAM, READ, WRITE
    } kind;
    union{
        struct{ Operand right, left;}assign;
        
        struct{ Operand op;} sinop;
        struct{ Operand result, op1, op2; char* sign} binop;
    }u;
    InterCode pre;
    InterCode next;
};

Operand defaultPlace;
InterCode begin;

void initCode();
void insertCode(InterCode code);
void printOp(Operand operand);
void printCode();

Operand createOperand();
InterCode createInterCode();

#endif