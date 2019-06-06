#include "targetCode.h"

struct Register_ regs[REG_NUM];
FILE* fp; 
const int stackSize = 100;
int currentReg = 0;
int spOffset = 0;
int currentParam = 0;
int currentArg = 0;
Var_t varList = NULL;

char *tempKindArray[] = {
        "CODE_BEGIN", 
        "ASSIGN", "OPERATION", 
        "GOTO", "IF_GOTO", 
        "FUNCTION_C", "RETURN_C", "ARG_C", 
        "DEC", "PARAM", "READ", "WRITE", "LABEL_C"
};

char *tempuKindArray[] = {
    "DEFAULT", "VAR", "CONSTANT_INT", "CONSTANT_FLOAT", "VAR2ADDR", "ADDR2VAR", "CALL", "_SIZE"
};

char* regsName[]={
    "$zero",
    "$at",
    "$v0","$v1",
    "$a0","$a1","$a2","$a3",
    "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
    "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
    "$t8","$t9",
    "$k0","$k1",
    "$gp",
    "$sp",
    "$fp",
    "$ra"
};

void printAllTargetCode(char* fileName){
    fp = fopen(fileName,"w");
    if(fp == NULL){
        printf("Error: Can not open the file \"%s\"", fileName);
        return ;
    }

    initRegs();
    fputs(".data\n", fp);
	fputs("_prompt: .asciiz \"Enter an integer:\"\n", fp);
	fputs("_ret: .asciiz \"\\n\"\n", fp);
	fputs(".globl main\n", fp);
	fputs(".text\n",fp);
	//read
	fputs("\nread:\n", fp);
	fputs("\tli $v0, 4\n", fp);
	fputs("\tla $a0, _prompt\n",fp);
	fputs("\tsyscall\n", fp);
	fputs("\tli $v0, 5\n", fp);
	fputs("\tsyscall\n", fp);
	fputs("\tjr $ra\n", fp);
	//write
	fputs("\nwrite:\n",fp);
	fputs("\tli $v0, 1\n", fp);
	fputs("\tsyscall\n", fp);
	fputs("\tli $v0, 4\n", fp);
	fputs("\tla $a0, _ret\n", fp);
	fputs("\tsyscall\n", fp);
	fputs("\tmove $v0, $0\n", fp);
	fputs("\tjr $ra\n\n", fp);


    InterCode interCode = begin->next;
    while(interCode->kind != CODE_BEGIN){
        printf("now kind is %s\n", tempKindArray[interCode->kind]);
        printTargetCode(interCode);
        interCode = interCode->next;
    }
}

void printTargetCode(InterCode interCode){
    switch(interCode->kind){
        case LABEL_C:
            targetCodeLabel(interCode);
            break;
        case ASSIGN:
            targetCodeAssign(interCode);
            break;
        case OPERATION:
            targetCodeOperation(interCode);
            break;
        case READ:
            targetCodeRead(interCode);
            break;
        case WRITE:
            targetCodeWrite(interCode);
            break;
        case RETURN_C:
            targetCodeReturn(interCode);
            break;
        case GOTO:
            targetCodeGoto(interCode);
            break;
        case IF_GOTO:
            targetCodeIfGoto(interCode);
            break;
        case FUNCTION_C:
            targetCodeFunction(interCode);
            break;
        case ARG_C:
            targetCodeArg(interCode);
            break;
        case PARAM:
            targetCodeParam(interCode);
            break;
        case DEC:
            targetCodeDec(interCode);
            break;
        default:
			printf("Error: Unknown Kind to MIPS\n");
			exit(-1);
        /*
		case ADDRESS_N:
			mipsAddress(interCode);
			break;
        */



    }
}

void targetCodeLabel(InterCode interCode){
    char str[STR_LENGTH];
    memset(str, 0, sizeof(str));
    //label x:
    sprintf(str, "label%s:\n", interCode->u.sinop.op->u.value);
    fputs(str, fp);
}

void targetCodeAssign(InterCode interCode){
    char str[STR_LENGTH];
    memset(str, 0, sizeof(str));
    Operand left = interCode->u.assign.left;
    Operand right = interCode->u.assign.right;
    if(right->kind == CALL){
        fputs("\tsubu $sp, $sp, 4\n", fp);
        fputs("\tsw $ra, 0($sp)\n", fp);

        int x = getReg(left);
        // jal f
        // move reg(x), $v0
        char str[STR_LENGTH];
        memset(str, 0, sizeof(str));
        sprintf(str, "\tjal %s\n\tmove %s, $v0\n", right->u.value, printReg(x));
        fputs(str, fp);
        swReg(x);
        fputs("\tlw $ra, 0($sp)\n", fp);
        fputs("\taddi $sp, $sp, 4\n", fp);
        currentArg = 0;
    }else{
        int x,y;
        if(left->kind == VAR){
            //x := #k
            if(right->kind == CONSTANT_INT || right->kind == CONSTANT_FLOAT){
                x = getReg(left);
                if(right->kind == CONSTANT_INT){
                    sprintf(str,"\tli %s, %d\n", printReg(x), right->u.intVar);
                }
                else if(right->kind == CONSTANT_FLOAT){
                    sprintf(str,"\tli %s, %f\n", printReg(x), right->u.floatVar);
                }
                fputs(str,fp);
            }else if(right->kind == VAR){//x := y
                x = getReg(left);
                y = getReg(right);
                sprintf(str,"\tmove %s, %s\n", printReg(x), printReg(y));
                fputs(str,fp);
            }else if(right->kind == ADDR2VAR){ // x:= *y
                x = getReg(left);
                y = getReg(right);
                sprintf(str,"\tlw %s, 0(%s)\n", printReg(x),printReg(y));
            }
        }else if(left->kind == ADDR2VAR){
            //*x = y
            if(right->kind == CONSTANT_FLOAT){
                //当等号右边为立即数时
                x = getReg(left);
                sprintf(str, "\tli $s3, %f\n\tsw $s3, 0(%s)\n", right->u.floatVar, printReg(x));
            }else if(right->kind == CONSTANT_INT){
                x = getReg(left);
                sprintf(str, "\tli $s3, %d\n\tsw $s3, 0(%s)\n", right->u.intVar, printReg(x)); 
            }else if(right->kind == VAR){
                x = getReg(left);
                y = getReg(right);
                sprintf(str, "\tsw %s, 0(%s)",printReg(y), printReg(x));
            }
        }
        swReg(x);
    }
}
void targetCodeOperation(InterCode interCode){
    char str[STR_LENGTH];
    memset(str,0, sizeof(str));
    Operand result = interCode->u.binop.result;
    Operand op1 = interCode->u.binop.op1;
    Operand op2 = interCode->u.binop.op2;
    char *sign = interCode->u.binop.sign;

    // printf("result kind is %s, op1 kind is %s, op2 kind is %s, sign is %s\n",tempuKindArray[result->kind],tempuKindArray[op1->kind],tempuKindArray[op2->kind],sign);
    int x,y,z;
    //PLUS MINUS STAR DIV 对应 + - * /
    if((op1->kind==VAR)&& (op2->kind==VAR)){    
        x = getReg(result);
        y = getReg(op1);
        z = getReg(op2);

        if(!strcmp(sign,"PLUS")){
            sprintf(str, "\tadd %s, %s, %s\n", printReg(x), printReg(y), printReg(z));
        }else if(!strcmp(sign,"MINUS")){
            sprintf(str, "\tsub %s, %s, %s\n", printReg(x), printReg(y), printReg(z));
        }else if(!strcmp(sign,"STAR")){
            sprintf(str, "\tmul %s, %s, %s\n", printReg(x), printReg(y), printReg(z));
        }else if(!strcmp(sign,"DIV")){
            sprintf(str, "\tdiv %s, %s\n\tmflo %s\n", printReg(y), printReg(z), printReg(x));
        }else{
            exit(-1);
        }
        fputs(str,fp);
    }else if((op1->kind == CONSTANT_FLOAT || op1->kind == CONSTANT_INT) && (op2->kind==VAR)){
        x = getReg(result);
        y = getReg(op2);
        if(!strcmp(sign,"PLUS")){
            if(op1->kind == CONSTANT_INT)
                sprintf(str, "\taddi %s, %s, %d\n", printReg(x), printReg(y), op1->u.intVar);
            else if(op1->kind == CONSTANT_FLOAT)
                sprintf(str, "\taddi %s, %s, %f\n", printReg(x), printReg(y), op1->u.floatVar);
            else exit(-1);
        }else if(!strcmp(sign, "MINUS")){
            if(op1->kind == CONSTANT_INT)
                sprintf(str, "\taddi %s, %s, -%d\n", printReg(x), printReg(y), op1->u.intVar);
            else if(op1->kind == CONSTANT_FLOAT)
                sprintf(str, "\taddi %s, %s, -%f\n", printReg(x), printReg(y), op1->u.floatVar);
            else exit(-1);
        }
        fputs(str,fp);
    }else if((op1->kind == VAR) && (op2->kind == CONSTANT_FLOAT || op2->kind == CONSTANT_INT)){
        x = getReg(result);
        y = getReg(op1);
        if(!strcmp(sign,"PLUS")){
            if(op2->kind == CONSTANT_INT){
                sprintf(str, "\taddi %s, %s, %d\n", printReg(x), printReg(y), op2->u.intVar);
            }
            else if(op2->kind == CONSTANT_FLOAT)
                sprintf(str, "\taddi %s, %s, %f\n", printReg(x), printReg(y), op2->u.floatVar);
            else exit(-1);
        }else if(!strcmp(sign,"MINUS")){
            if(op2->kind == CONSTANT_INT)
                sprintf(str, "\taddi %s, %s, -%d\n", printReg(x), printReg(y), op2->u.intVar);
            else if(op2->kind == CONSTANT_FLOAT)
                sprintf(str, "\taddi %s, %s, -%f\n", printReg(x), printReg(y), op2->u.floatVar);
            else exit(-1);
        }
        fputs(str, fp);
    }
    swReg(x);
}
void targetCodeRead(InterCode interCode){
    fputs("\tsubu $sp, $sp, 4\n", fp);
	fputs("\tsw $ra, 0($sp)\n", fp);

	int x = getReg(interCode->u.sinop.op);
	char str[STR_LENGTH];
	memset(str, 0, sizeof(str));
	sprintf(str, "\tjal read\n\tmove %s, $v0\n", printReg(x));
	fputs(str, fp);
	swReg(x);

	fputs("\tlw $ra, 0($sp)\n", fp);
	fputs("\taddi $sp, $sp, 4\n", fp);
}
void targetCodeWrite(InterCode interCode){
    fputs("\tsubu $sp, $sp, 4\n", fp);
	fputs("\tsw $ra, 0($sp)\n", fp);

	char str[STR_LENGTH];
	memset(str, 0, sizeof(str));
	int r = getReg(interCode->u.sinop.op);
	if(interCode->u.sinop.op->kind == VAR)
		sprintf(str, "\tmove $a0, %s\n\tjal write\n", printReg(r));
	else if(interCode->u.sinop.op->kind == ADDR2VAR){
		sprintf(str, "\tlw $a0, 0(%s)\n\tjal write\n", printReg(r));
	}
	fputs(str, fp);
	swReg(r);

	fputs("\tlw $ra, 0($sp)\n", fp);
	fputs("\taddi $sp, $sp, 4\n", fp);
}
void targetCodeReturn(InterCode interCode){
    // printf("now step in return\n");
	char str[STR_LENGTH];
	memset(str, 0, sizeof(str));
	Operand operand = interCode->u.sinop.op;
    if(!(operand->kind == CONSTANT_INT || operand->kind==CONSTANT_FLOAT)){
        // printf("now here\n");
        int x = getReg(operand);
        // printf("x value is %d\n", x);
        sprintf(str, "\tmove $v0, %s\n\taddi $sp, $sp, %d\n\tlw $fp, 0($sp)\n\taddi $sp, $sp, 4\n\tjr $ra\n", printReg(x), stackSize);
    }else{
        if(operand->kind == CONSTANT_INT){
            sprintf(str, "\tmove $v0, $%d\n\taddi $sp, $sp, %d\n\tlw $fp, 0($sp)\n\taddi $sp, $sp, 4\n\tjr $ra\n", operand->u.intVar, stackSize);
        }else if(operand->kind == CONSTANT_FLOAT){
            sprintf(str, "\tmove $v0, $%f\n\taddi $sp, $sp, %d\n\tlw $fp, 0($sp)\n\taddi $sp, $sp, 4\n\tjr $ra\n", operand->u.floatVar, stackSize);
        }
    }
    fputs(str,fp);
}
void targetCodeGoto(InterCode interCode){
    char str[STR_LENGTH];
	memset(str, 0, sizeof(str));
    sprintf(str, "\tj label%s\n",interCode->u.sinop.op->u.value);
	fputs(str, fp);
}
void targetCodeIfGoto(InterCode interCode){
    char str[STR_LENGTH];
	memset(str, 0, sizeof(str));
    Operand left = interCode->u.if_goto.t1;
    Operand right = interCode->u.if_goto.t2;
    Operand label = interCode->u.if_goto.label;
    char* op = interCode->u.if_goto.op;
    // printf("if_goto operand is %s\n", op);

    if((left->kind !=CONSTANT_FLOAT &&left->kind!=CONSTANT_INT) && (right->kind!=CONSTANT_INT && right->kind!=CONSTANT_FLOAT)){
        int x = getReg(left);
        int y = getReg(right);
        if(!strcmp(op,"==")){
            sprintf(str, "\tbeq %s, %s, label%s\n",printReg(x), printReg(y), label->u.value);
        }else if(!strcmp(op, "!=")){
			sprintf(str, "\tbne %s, %s, label%s\n",printReg(x), printReg(y), label->u.value);
		} else if(!strcmp(op, ">")){
			sprintf(str, "\tbgt %s, %s, label%s\n",printReg(x), printReg(y), label->u.value);
		} else if(!strcmp(op, "<")){
			sprintf(str, "\tblt %s, %s, label%s\n",printReg(x), printReg(y), label->u.value);
		} else if(!strcmp(op, ">=")){
			sprintf(str, "\tbge %s, %s, label%s\n",printReg(x), printReg(y), label->u.value);
		} else if(!strcmp(op, "<=")){
			sprintf(str, "\tble %s, %s, label%s\n",printReg(x), printReg(y), label->u.value);
		}
    }else if((left->kind == CONSTANT_FLOAT || left->kind == CONSTANT_INT) && (right->kind != CONSTANT_FLOAT && right->kind != CONSTANT_INT)){
        int y = getReg(right);
		// bxx reg(x), reg(y), z
		if(!strcmp(op, "==")){
            if(left->kind == CONSTANT_INT){
                sprintf(str, "\tbeq %s, %d, label%s\n", printReg(y), left->u.intVar, label->u.value);
            }else if(left->kind == CONSTANT_FLOAT){
                sprintf(str, "\tbeq %s, %f, label%s\n", printReg(y), left->u.floatVar, label->u.value);
            }	
		} else if(!strcmp(op, "!=")){
            if(left->kind == CONSTANT_INT){
                sprintf(str, "\tbne %s, %d, label%s\n", printReg(y), left->u.intVar, label->u.value);
            }else if(left->kind == CONSTANT_FLOAT){
                sprintf(str, "\tbne %s, %f, label%s\n", printReg(y), left->u.floatVar, label->u.value);
            }
		} else if(!strcmp(op, ">")){
            if(left->kind == CONSTANT_INT){
                sprintf(str, "\tblt %s, %d, label%s\n", printReg(y), left->u.intVar, label->u.value);
            }else if(left->kind == CONSTANT_FLOAT){
                sprintf(str, "\tblt %s, %f, label%s\n", printReg(y), left->u.floatVar, label->u.value);
            }
		} else if(!strcmp(op, "<")){
            if(left->kind == CONSTANT_INT){
                sprintf(str, "\tbgt %s, %d, label%s\n", printReg(y), left->u.intVar, label->u.value);
            }else if(left->kind == CONSTANT_FLOAT){
                sprintf(str, "\tbgt %s, %f, label%s\n", printReg(y), left->u.floatVar, label->u.value);
            }
		} else if(!strcmp(op, ">=")){
			if(left->kind == CONSTANT_INT){
                sprintf(str, "\tble %s, %d, label%s\n", printReg(y), left->u.intVar, label->u.value);
            }else if(left->kind == CONSTANT_FLOAT){
                sprintf(str, "\tble %s, %f, label%s\n", printReg(y), left->u.floatVar, label->u.value);
            }
		} else if(!strcmp(op, "<=")){
			if(left->kind == CONSTANT_INT){
                sprintf(str, "\tbge %s, %d, label%s\n", printReg(y), left->u.intVar, label->u.value);
            }else if(left->kind == CONSTANT_FLOAT){
                sprintf(str, "\tbge %s, %f, label%s\n", printReg(y), left->u.floatVar, label->u.value);
            }
		}
	}else if((right->kind == CONSTANT_FLOAT || right->kind == CONSTANT_INT )&& (left->kind != CONSTANT_FLOAT && left->kind != CONSTANT_INT)){
		int x = getReg(left);
		// bxx reg(x), reg(y), z
		if(!strcmp(op, "==")){
            if(right->kind == CONSTANT_INT){
                sprintf(str, "\tbeq %s, %d, label%s\n",printReg(x), right->u.intVar, label->u.value);
            }else if(right->kind == CONSTANT_FLOAT){
                sprintf(str, "\tbeq %s, %f, label%s\n",printReg(x), right->u.floatVar, label->u.value);
            }	
		} else if(!strcmp(op, "!=")){
			if(right->kind == CONSTANT_INT){
                sprintf(str, "\tbne %s, %d, label%s\n",printReg(x), right->u.intVar, label->u.value);
            }else if(right->kind == CONSTANT_FLOAT){
                sprintf(str, "\tbne %s, %f, label%s\n",printReg(x), right->u.floatVar, label->u.value);
            }	
		} else if(!strcmp(op, ">")){
			if(right->kind == CONSTANT_INT){
                sprintf(str, "\tbgt %s, %d, label%s\n",printReg(x), right->u.intVar, label->u.value);
            }else if(right->kind == CONSTANT_FLOAT){
                sprintf(str, "\tbgt %s, %f, label%s\n",printReg(x), right->u.floatVar, label->u.value);
            }	
		} else if(!strcmp(op, "<")){
			if(right->kind == CONSTANT_INT){
                sprintf(str, "\tblt %s, %d, label%s\n",printReg(x), right->u.intVar, label->u.value);
            }else if(right->kind == CONSTANT_FLOAT){
                sprintf(str, "\tblt %s, %f, label%s\n",printReg(x), right->u.floatVar, label->u.value);
            }	
		} else if(!strcmp(op, ">=")){
			if(right->kind == CONSTANT_INT){
                sprintf(str, "\tbge %s, %d, label%s\n",printReg(x), right->u.intVar, label->u.value);
            }else if(right->kind == CONSTANT_FLOAT){
                sprintf(str, "\tbge %s, %f, label%s\n",printReg(x), right->u.floatVar, label->u.value);
            }	
		} else if(!strcmp(op, "<=")){
			if(right->kind == CONSTANT_INT){
                sprintf(str, "\tble %s, %d, label%s\n",printReg(x), right->u.intVar, label->u.value);
            }else if(right->kind == CONSTANT_FLOAT){
                sprintf(str, "\tble %s, %f, label%s\n",printReg(x), right->u.floatVar, label->u.value);
            }	
		}
	}
	fputs(str, fp);
}
void targetCodeFunction(InterCode interCode){
	char str[STR_LENGTH];
	memset(str, 0, sizeof(str));
	// function:
	sprintf(str, "%s:\n\tsubu $sp, $sp, 4\n\tsw $fp, 0($sp)\n\tmove $fp, $sp\n\tsubu $sp, $sp, %d\n",interCode->u.sinop.op->u.value,stackSize);
	fputs(str, fp);
	spOffset = 0;
	currentParam = 0;
}
void targetCodeArg(InterCode interCode){
    char str[STR_LENGTH];
    memset(str,0,sizeof(str));
    Operand operand = interCode->u.sinop.op;
    Var_t arg = NULL;
    if(operand->kind == VAR){
        arg = findVar(operand->u.value);
    }
    if(arg == NULL){
        exit(-1);
    }
    if(currentArg < 4){
        sprintf(str, "\tlw $a%d, %d($fp)\n", currentArg, arg->offset);
    }else{
        sprintf(str, "\tlw $s0, %d($fp)\n\tsubu $sp, $sp, 4\n\tlw $s0, 0($sp)\n", arg->offset);
    }
    fputs(str,fp);
    ++currentArg;
    if(interCode->next == NULL || interCode->next->kind != ARG_C)
        currentArg=0;
}
void targetCodeParam(InterCode interCode){
    char str[STR_LENGTH];
	memset(str, 0, sizeof(str));
	Var_t param = malloc(sizeof(struct Var_t_));  
	param->name = interCode->u.sinop.op->u.value;
	spOffset -= 4;
	param->offset = spOffset;
	addVar(param);
	if(currentParam<4){
		sprintf(str, "\tsw $a%d, %d($fp)\n", currentParam, param->offset);
	} else {
		sprintf(str, "\tlw $a0, %d($fp)\n\tsw $a0, %d($fp)\n", (currentParam-2)*4, param->offset);
	}
	fputs(str, fp);
	++currentParam;
}
void targetCodeDec(InterCode interCode){
    Var_t arrayHead = malloc(sizeof(struct Var_t_));
    spOffset -= 4;
    arrayHead->offset = spOffset;
    Operand op1 = interCode->u.binop.op1;
    Operand op2 = interCode->u.binop.op2;
    int size = op2->u.intVar;
    spOffset -= size;
    if(op1->kind == VAR){
        arrayHead->name = op1->u.value;
    }
    addVar(arrayHead);

    char str[STR_LENGTH];
    memset(str,0,sizeof(str));
    sprintf(str, "\taddi $s1, $fp, %d\n\tsw $s1, %d($fp)\n", spOffset, arrayHead->offset);
	fputs(str, fp);
}


void initRegs(){  
    for(int i=0; i< REG_NUM; i++){
        regs[i].name = regsName[i];
        regs[i].var = NULL;
    }
}

int getReg(Operand operand){
    // printf("operand kind is %s\n", tempuKindArray[operand->kind]);
    char* name = NULL;
    if(operand->kind == VAR){
        // printf("var name is %s \n",operand->u.value);
        name = operand->u.value;
    }else if(operand->kind == ADDR2VAR){
        name = operand->u.value;
    }

    Var_t var = findVar(name);
    int i = currentReg + REG_T_START;
    currentReg = (++currentReg) % (REG_T_END);

    if(var == NULL){
        // printf("getReg yes\n");
        var = malloc(sizeof(struct Var_t_));
        var->name = name;
        spOffset -= 4;
        var->offset = spOffset;
        addVar(var);
        var->reg = i;
        regs[i].var = var;
    }else{
        var->reg = i;
        regs[i].var = var;
        lwReg(i,var);
    }
    return i;
}

char* printReg(int index){
	return regs[index].name;
}

void swReg(int index){
	char str[50];
	memset(str, 0, sizeof(str));
	Var_t var = regs[index].var;
	sprintf(str, "\tsw %s, %d($fp)\n", printReg(index), var->offset);
	fputs(str, fp);
}

void lwReg(int index, Var_t var){
	char str[50];
	memset(str, 0, sizeof(str));
	regs[index].var = var;
	sprintf(str, "\tlw %s, %d($fp)\n", printReg(index), var->offset);
	fputs(str, fp);
}

void addVar(Var_t var){
    if(var==NULL)
		exit(-1);
	var->next = NULL;
	if(varList==NULL){
		varList = var;
	} else{
		Var_t ptr = varList;
		while(ptr->next!=NULL)
			ptr = ptr->next;
		ptr->next = var;
	}
}

Var_t findVar(char* name){
    Var_t ptr = varList; 
    while(ptr != NULL){
        // printf("loop in findVar, and name is %s, arg name is %s\n", ptr->name, name);
        if(!strcmp(ptr->name, name)){
            break;
        } else{
            ptr = ptr->next;
        }
    }
    return ptr;
}





