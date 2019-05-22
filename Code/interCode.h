#ifndef INTER_CODE_H
#define INTER_CODE_H
#include <stdio.h>
#include <memory.h>

typedef struct Operand_* Operand;
struct Operand_ {
    enum{ VAR, CONSTANT, VAR2ADDR, ADDR2VAR }kind;
    union{
        int intVar;
        float floatVar;
        char* value;
    }u;
};

typedef struct InterCode_* InterCode; 
struct InterCode_ {
    enum {CODE_BEGIN, ASSIGN, ADD, SUB, MUL, DIV, 
        GOTO, IF_GOTO, FUNCTION_C, RETURN_C, DEC, PARAM, READ, WRITE, CALL, LABEL} kind;
    union{
        struct{ Operand right, left;}assign; 
        struct{ Operand op;} sinop;
        struct{ Operand result, op1, op2;} binop;
        struct{ Operand t1, t2, label; char* op;} if_goto;
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