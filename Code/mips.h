#ifndef MIPS_H
#define MIPS_H

#define REG_NUM 32
#define STR_LENGTH 200
#define REG_T_START 8
#define REG_T_END 16

#include "interCode.h"
#include <stdio.h>
#include <stdlib.h>




typedef struct Var_t_* Var_t;
struct Var_t_{
    char* name;
    int reg;
    int offset;
    struct Var_t_* next;
};


typedef struct Register_* Register;
struct Register_{
    char* name;
    Var_t var;
};



void printMips(char* fileName);
void printTargetCode(InterCode interCode);

void targetCodeLabel(InterCode interCode);
void targetCodeAssign(InterCode interCode);
void targetCodeOperation(InterCode interCode);
void targetCodeRead(InterCode interCode);
void targetCodeWrite(InterCode interCode);
void targetCodeReturn(InterCode interCode);
void targetCodeGoto(InterCode interCode);
void targetCodeIfGoto(InterCode interCode);
void targetCodeFunction(InterCode interCode);
void targetCodeArg(InterCode interCode);
void targetCodeParam(InterCode interCode);
void targetCodeDec(InterCode interCode);

void initRegs();
int getReg(Operand operand);
char* printReg(int index);
void swReg(int index);
void lwReg(int index, Var_t var);
void addVar(Var_t var);
Var_t findVar(char* name);

#endif
