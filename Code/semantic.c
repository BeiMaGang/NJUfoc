#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "semantic.h"
#include "symbolTable.h"
#include "syntaxTree.h"

// #define DEBUG

int T = 0;
int V = 0;
int L = 0;
const int LEN = 5;
int ARRAY_LEVEL = 0;
Type NOW_ARRAY;

int PARAM_DEC_ING = 0;

InterCode argCodes[128];
int sizeOfCodes = 0;

extern Term SymbolTable[SIZE];
extern struct TreeNode* root;

int STATUS;
int STRUCTURE_LEVEL = 0;

char INFOR_BUFFER[1024];

char* infoCat(const char* str1, char* str2){
    INFOR_BUFFER[0] = '\0';
    strcat(INFOR_BUFFER, str1);
    strcat(INFOR_BUFFER, str2);
    return INFOR_BUFFER;
}

void debug(struct TreeNode* node, char* name){
#ifdef DEBUG
    printf(name);
    printf("\n");
    assert(node != NULL);
    assert(!strcmp(node->name, name));
#endif
}

void error(int type, int line, char* info){
    printf("Error type %d at Line %d: %s\n", type, line, info);
}

//finished
void Program(){
    assert(!strcmp(root->name, "Program"));
    if(root == NULL){
        return ;
    }
    ExtDefList(root->subTree);
}

//finished
void ExtDefList(struct TreeNode* node){
    debug(node, "ExtDefList");
    node = node->subTree;

    //ExtDefList -> null
    if(node == NULL)
        return;

    //ExtDefList -> ExtDef ExtDefList
    ExtDef(node);
    node = node->broTree;
    ExtDefList(node);
}

//finished
void ExtDef(struct TreeNode* node){
    debug(node, "ExtDef");

    node = node->subTree;
    Type inhType = Specifier(node);

    node = node->broTree;
    //ExtDef -> Specifier SEMI
    if(!strcmp(node->name, "SEMI")){
        return;
    }else if(!strcmp(node->name, "ExtDecList")){
        //ExtDef -> Specifier ExtDecList SEMI 
        ExtDecList(node, inhType);
        return;
    }else{
        //EXtDef -> Specifier FunDec CompSt
        FunDec(node, inhType);
        CompSt(node->broTree, inhType);
        return 0;
    }
}

//finished lab3 finished
void FunDec(struct TreeNode* node, Type type){
    debug(node, "FunDec");

    Funtion function = (Funtion)malloc(sizeof(struct Funtion_));
    function->retType = type;
    node = node->subTree;
    char* fun_name = node->idType;

    if(getTable(fun_name) != NULL){
        //error 4
        error(4, node->line, infoCat("Redefined function ", fun_name));
        return;
    }
    function->name = fun_name;

    //intercode
    InterCode funcCode = createInterCode();
    funcCode->kind = FUNCTION_C;
    Operand operand = createOperand();
    operand->kind = VAR;
    operand->u.value = fun_name;
    funcCode->u.sinop.op = operand;

    if(!strcmp(node->broTree->broTree->name, "VarList")){
        //FunDec -> ID LP VarList RP
        PARAM_DEC_ING = 1;
        function->paras = VarList(node->broTree->broTree);
        PARAM_DEC_ING = 0;
    }else{
        function->paras = NULL;
    }
    Type funType = (Type)malloc(sizeof(struct Type_));
    funType->kind = FUNTION;
    funType->u.funtion = function;
    varInsertTable(fun_name, funType);
}

//finished
void CompSt(struct TreeNode* node, Type returnType){
    debug(node, "CompSt");

    //CompSt -> LC DefList StmtList RC
    node = node->subTree->broTree;

    // node is DefList
    DefList(node);

    node = node->broTree;

    //node is StmtList
    StmtList(node, returnType);
}

//finished
void StmtList(struct TreeNode* node, Type type){
    debug(node, "StmtList");

    node = node->subTree;
    
    //StmtList -> NULL
    if(node == NULL)
        return;

    //StmtList -> Stmt StmtList

    Stmt(node, type);
    StmtList(node->broTree, type);
}

//finished
void Stmt(struct TreeNode* node, Type type){
    debug(node, "Stmt");

    node = node->subTree;
    if(!strcmp(node->name, "Exp")){
        //Stmt-> Exp
        Exp(defaultPlace, node);
    }else if(!strcmp(node->name, "CompSt")){
        CompSt(node, type);
    }else if(!strcmp(node->name, "RETURN")){
        Type returnType = Exp(defaultPlace, node->broTree);
        if(!typeEqual(type, returnType)){
            //error 8
            error(8, node->line, "Type mismatched for return");
        }
    }else if(!strcmp(node->name, "IF")){
        node = node->broTree->broTree;
        //node is Exp;
        assert(!strcmp(node->name, "Exp"));

        //Stmt -> IF LP Exp RP Stmt
        Type ifType = Exp(defaultPlace, node);
        if(ifType->kind != BASIC || ifType->u.basic != TYPE_INT){
            //error 7
            error(7, node->line, "if judge condition only int");
        }

        node = node->broTree->broTree;
        //node is Stmt
        assert(!strcmp(node->name, "Stmt"));
        Stmt(node, type);

        if(node->broTree!=NULL){
            Stmt(node->broTree->broTree, type);
        }
    }else if(!strcmp(node->name, "WHILE")){
        // Stmt -> WHILE LP Exp RP Stmt 
        node = node->broTree->broTree;
        //node is Exp;
        assert(!strcmp(node->name, "Exp"));
        
        Type ifType = Exp(defaultPlace, node);
        if(ifType->kind != BASIC || ifType->u.basic != TYPE_INT){
            //error 7
            error(7, node->line, "while judge condition only int");
        }
        Stmt(node->broTree->broTree, type);
    }
}

//finished
void ExtDecList(struct TreeNode* node, Type type){
    debug(node, "ExtDecList");
    node = node->subTree;
    VarDec(node, type);

    if(node->broTree == NULL)
        return;
    //ExtDecList -> VarDec , ExtDecList
    node = node->broTree->broTree;
    ExtDecList(node, type);
}

//finished
Type Specifier(struct TreeNode* node){
    debug(node, "Specifier");

    Type retType = (Type)malloc(sizeof(Type));
    node = node->subTree;
    if(!strcmp(node->name, "TYPE")){
        retType->kind = BASIC;
        if(!strcmp(node->idType, "int")){
            retType->u.basic = TYPE_INT;
        }else{
            retType->u.basic = TYPE_FLOAT;
        }
    }else{
        //Specifier -> StructSpecifier
        free(retType);
        retType = StructSpecifier(node);
    }
    return retType;
}

//finished
Type StructSpecifier(struct TreeNode* node){
    debug(node, "StructSpecifier");
    struct TreeNode* child = node->subTree;
    /*
    StructSpecifier : STRUCT OptTag LC DefList RC
        | STRUCT Tag
    */
   Type type;
   if(!strcmp(child->broTree->name, "OptTag")){
       //StructSpecifier -> STRUCT OptTag LC DefList RC

        STRUCTURE_LEVEL++;

        child = child->broTree;
        Structure structure = (Structure)malloc(sizeof(struct Structure_));
        structure->fieldList = DefList(child->broTree->broTree);

        STRUCTURE_LEVEL--;
        //new struct type
        type = (Type)malloc(sizeof(struct Type_));
        type->kind = STRUCTURE;
        type->u.structure = structure;
        if(child->subTree == NULL){
            //OptTag -> NULL
            structure->name = NULL;
        }else{
            structure->name = child->subTree->idType;
            if(checkNotExist(structure->name)){
                structInsertTable(type);
            }else{
                //error 16
                error(16, child->subTree->line, infoCat("struct Duplicated name: ",structure->name));
            }
        }
   }else{
       //StructSpecifier -> STRUCT Tag
       char* name = child->broTree->subTree->idType;
       type = getTable(name);
       if(type == NULL){
           //error 17
           error(17, child->broTree->subTree->line, infoCat("Undefined structure ", name));
           return DEFAULT_TYPE;
       }
   }
   return type;
}

//finished
FieldList VarList(struct TreeNode* node){
    debug(node, "VarList");

    node = node->subTree;
    FieldList list = NULL;
    list = ParamDec(node);

    if(node->broTree != NULL){
        // ParamDec, VarList
        list->tail = VarList(node->broTree->broTree);
    }
    return list;
}

//finished
FieldList ParamDec(struct TreeNode* node){
    debug(node, "ParamDec");

    node = node->subTree;
    Type type = Specifier(node);
    //ParamDec -> Specifier VarDec
    return VarDec(node->broTree, type);

}

//finished
FieldList DefList(struct TreeNode* node){
    debug(node, "DefList");
    
    //DefList-> NULL
    assert(node != NULL);
    node = node->subTree;

    if(node == NULL)
        return NULL;

    //->Def DefList
    FieldList field = Def(node);
    if(field == NULL){
        return DefList(node->broTree);
    }
    node = node->broTree;
    FieldList p = field;
    while(p->tail != NULL)
        p = p->tail; 

    p->tail = DefList(node);
    return field;
}


//finished
FieldList Def(struct TreeNode* node){
    debug(node, "Def");
    
    //Def     : Specifier DecList SEMI
    FieldList field;
    node = node->subTree;
    Type type = Specifier(node);

    if(type == NULL){
        type = DEFAULT_TYPE;
    }
    
    field = DecList(node->broTree, type);
    return field;
}


//finished
FieldList DecList(struct TreeNode* node,Type type){
    debug(node, "DecList");
    /*
     * DecList : Dec {$$ = createNode("DecList",1,$1);}
        | Dec COMMA DecList {$$ = createNode("DecList",3,$1,$2,$3);}
     */
    assert(!strcmp(node->name, "DecList"));
    FieldList rnt = NULL;
    node = node->subTree;

    rnt = Dec(node, type);
    node = node->broTree;
    if(node == NULL){
        //Dec
        return rnt;
    }else{
        //Dec COMMA DecList
        node = node->broTree;
        if(rnt == NULL){
            rnt = DecList(node, type);
        }else{
            FieldList tmpDec = rnt;
            while(tmpDec->tail != NULL)
                    tmpDec = tmpDec->tail;
            tmpDec->tail = DecList(node, type);
        }
    }
    return rnt;
}

//ing
FieldList Dec(struct TreeNode* node,Type type){
    debug(node, "Dec");

    node = node->subTree;
    if(node->broTree == NULL){
        //Dec->VarDec
        return VarDec(node, type);
    }else{
        //Dec -> VarDec ASSIGNOP Exp
        if(STRUCTURE_LEVEL > 0){
            //error 15
            error(15, node->broTree->line, "initial in Struct Field");
        }
        FieldList field = VarDec(node, type);
        Type dstType = Exp(defaultPlace, node->broTree->broTree);
        if(!typeEqual(field->type, dstType)){
            //error 5
            error(5, node->broTree->line, "Type mismatched for assignment");
        }
        return field;
    }
}

//finished lab3 finished
FieldList VarDec(struct TreeNode* node,Type type){
    debug(node, "VarDec");
    node = node->subTree;

    FieldList field = NULL;
    if(!strcmp(node->name, "ID")){
        //VarDec -> ID
        field = (FieldList)malloc(sizeof(struct FieldList_));
        field->tail = NULL;
        field->name = node->idType;
        field->type = type;

        //intercode decCode
        if(type->kind == ARRAY && PARAM_DEC_ING == 0){
            InterCode decCode = createInterCode();
            decCode->kind = DEC;

            Operand op1 = createOperand();
            op1->kind = VAR;
            op1->u.value = node->idType;

            decCode->u.binop.op1 = op1;

            Operand op2 = createOperand();
            op2->kind = _SIZE;
            op2->u.intVar = sizeArray(type);

            decCode->u.binop.op2 = op2;
        }

        //intercode
        if(PARAM_DEC_ING == 1){
            InterCode paramCode = createInterCode();
            paramCode->kind = PARAM;
            Operand param = createOperand();
            param->kind = VAR;
            param->u.value = node->idType;
            paramCode->u.sinop.op = param;
        }

        if(getTable(node->idType) == NULL){
            varInsertTable(node->idType, type);
        }else{
            //error 3 15
            if(STRUCTURE_LEVEL > 0){
                //error 15
                error(15, node->line, infoCat("Redefined field: ",node->idType));
            }else{
                //error 3
                error(3, node->line, infoCat("Variable Redefinition ", node->idType));
            }
        }
    }else{
        //VarDec LB INT RB
        Type nowType = (Type)malloc(sizeof(struct Type_));
        nowType->kind = ARRAY;
        nowType->u.array.size = node->broTree->broTree->intType;
        nowType->u.array.elem = type;
        return VarDec(node, nowType);
    }
    return field;
}

//finished  lab3 ing
Type Exp(Operand place, struct TreeNode* node){
    debug(node, "Exp");
    node = node->subTree;

    Type retType = NULL;
    if(!strcmp(node->name, "FLOAT")){
        //FLOAT
        retType = (Type)malloc(sizeof(struct Type_));
        retType->kind = BASIC;
        retType->u.basic = TYPE_FLOAT;
        retType->assign = RIGHT;

        // intercode
        place->kind = CONSTANT_FLOAT;
        place->u.floatVar = node->floatType;

        return retType;
    }else if(!strcmp(node->name, "INT")){
        //INT
        retType = (Type)malloc(sizeof(struct Type_));
        retType->kind = BASIC;
        retType->u.basic = TYPE_INT;
        retType->assign = RIGHT;

        // intercode
        place->kind = CONSTANT_INT;
        place->u.intVar = node->intType;

        return retType;
    }else if(!strcmp(node->name, "ID")){
        //Exp -> ID ...
        if(node->broTree == NULL){
            //Exp -> ID
            Type typeID = getTable(node->idType);
            if(typeID->kind == ARRAY){
                NOW_ARRAY = typeID;
            }
            if(typeID == NULL){
                // error 1
                error(1, node->line, infoCat("UnDefined variable ", node->idType));
                return DEFAULT_TYPE;
            }
            retType = (Type)malloc(sizeof(struct Type_));
            memcpy(retType, typeID, sizeof(struct Type_));
            retType->assign = LEFT;

            //interCode finished
            place->kind = VAR;
            place->u.value = node->idType;

            return retType;
        }else if(!strcmp(node->broTree->name, "LP")){
            // Exp -> ID LP Args RP | ID LP RP
            Type func = getTable(node->idType);
            if(func == NULL){
                // error 2
                error(2, node->line, infoCat("Undefined Function ", node->idType));
                return DEFAULT_TYPE;
            }else if(func->kind != FUNTION){
                //error 11
                error(11, node->line, infoCat(node->idType, " is not a function"));
                return DEFAULT_TYPE;
            }
            FieldList param = func->u.funtion->paras;
            node = node->broTree->broTree;
            if(!strcmp(node->name, "RP")){
                //ID LP RP
                if(param != NULL){
                    //error 9
                    error(9, node->line, infoCat("Function param is not matched :", getFuncInf(func->u.funtion)));
                    return DEFAULT_TYPE;
                }
            }else{
                //ID LP Args RP
                sizeOfCodes = 0;
                if(Args(node, param) == 0){
                    //error 9
                    error(9, node->line, infoCat("Function param is not matched :", getFuncInf(func->u.funtion)));
                    return DEFAULT_TYPE;
                }
                int i = sizeOfCodes - 1;
                while(i >= 0){
                    insertCode(argCodes[i]);
                    i--;
                }
            }

            //intercode 
            place->kind = CALL;
            place->u.value = func->u.funtion->name;
            retType = (Type)malloc(sizeof(struct Type_));
            memcpy(retType, func->u.funtion->retType, sizeof(struct Type_));
            retType->assign = RIGHT;
            return retType;
        }
    }else if(!strcmp(node->name, "Exp")){
        //Exp -> Exp ...
        struct TreeNode* fstChild = node;
        node = node->broTree;
        if(!strcmp(node->name, "ASSIGNOP")){

            Operand t1 = createOperand();
            t1->kind = VAR;
            t1->u.value = new_temp();
            Type p1 = Exp(t1, fstChild);
            //Exp -> Exp ASSIGNOP Exp

            //intercode
            Operand t2 = createOperand();
            t2->kind = VAR;
            t2->u.value = new_temp();
            Type p2 = Exp(t2, node->broTree);
            InterCode assignCode = createInterCode();
            assignCode->kind = ASSIGN;
            assignCode->u.assign.left = t1;
            assignCode->u.assign.right = t2;

            if(p1->assign != LEFT){
                //error 6
                error(6, node->line, "The left-hand side of an assignment must be a variable");
                return DEFAULT_TYPE;
            }
            if(!typeEqual(p1, p2)){
                //error 5
                error(5, node->line, "Type mismatched for assignment");
                return DEFAULT_TYPE;
            }
            return p1;
        }else if(!strcmp(node->name, "AND")||
                !strcmp(node->name, "OR")||
                !strcmp(node->name, "RELOP")){
            //Exp -> Exp AND|OR|RELOP Exp
            Type p1 = Exp(defaultPlace, fstChild);
            Type p2 = Exp(defaultPlace, node->broTree);
            if(!(p1->kind == BASIC && p1->u.basic == TYPE_INT && 
                p2->kind == BASIC && p2->u.basic == TYPE_INT)){
                //error 7
                error(7, node->line, "Type mismatched for operands, Only int");
                return DEFAULT_TYPE;
            }
            retType = (Type)malloc(sizeof(struct Type_));
            memcpy(retType, p1, sizeof(struct Type_));
            retType->assign = RIGHT;
            return retType;
        }else if(!strcmp(node->name, "PLUS")||
                !strcmp(node->name, "MINUS")||
                !strcmp(node->name, "STAR")||
                !strcmp(node->name, "DIV")){
            // Exp -> Exp +-*/ Exp
            
            //intercode
            Operand t1 = createOperand();
            Operand t2 = createOperand();
            t1->kind = VAR;
            t2->kind = VAR;
            t1->u.value = new_temp();
            t2->u.value = new_temp();

            Type p1 = Exp(t1, fstChild);
            Type p2 = Exp(t2, node->broTree);
            
            InterCode calCode = createInterCode();
            calCode->kind = OPERATION;
            calCode->u.binop.op1 = t1;
            calCode->u.binop.op2 = t2;
            calCode->u.binop.result = place;
            calCode->u.binop.sign = node->name;


            if(typeEqual(p1, p2) && p1->kind == BASIC){
                retType = (Type)malloc(sizeof(struct Type_));
                memcpy(retType, p1, sizeof(struct Type_));
                retType->assign = RIGHT;
                return retType;
            }else{
                //error 7
                error(7, node->line, "Type mismatched for operands, Only int or float");
                return DEFAULT_TYPE;
            }
        }else if(!strcmp(node->name, "LB")){
            // Exp -> Exp LB Exp RB


            //intercode finished
            Operand t1 = createOperand();
            t1->u.value = new_temp();
            t1->kind = VAR;
            Type p1 = Exp(t1, fstChild);

            ARRAY_LEVEL++;
            if(ARRAY_LEVEL == 1 && !isParam(t1->u.value)){
                t1->kind = VAR2ADDR;
            }
            Operand t2 = createOperand();
            t2->kind = VAR;
            t2->u.value = new_temp();
            Type p2 = Exp(t2, node->broTree);

            place->kind = ADDR2VAR;

            InterCode starCode = createInterCode();
            starCode->kind = OPERATION;
            starCode->u.binop.op1 = t2;

            assert(NOW_ARRAY != NULL);
            NOW_ARRAY = NOW_ARRAY->u.array.elem;

            Operand rightStar = createOperand();
            rightStar->kind = CONSTANT_INT;
            rightStar->u.intVar = sizeArray(NOW_ARRAY);

            starCode->u.binop.op2 = rightStar;
            Operand result = createOperand();
            result->kind = VAR;
            result->u.value = new_temp();
            starCode->u.binop.result = result;
            starCode->u.binop.sign = "STAR";

            InterCode plusCode = createInterCode();
            plusCode->kind = OPERATION;

            Operand resultPlus = createOperand();
            memcpy(resultPlus, place, sizeof(struct Operand_));
            resultPlus->kind = VAR;

            Operand leftPlus = createOperand();
            memcpy(leftPlus, t1, sizeof(struct Operand_));
            leftPlus->kind = VAR;
            if(ARRAY_LEVEL == 1 && !isParam(t1->u.value))
                leftPlus->kind = VAR2ADDR;

            plusCode->u.binop.op1 = leftPlus;
            plusCode->u.binop.op2 = result;
            plusCode->u.binop.result = resultPlus;
            plusCode->u.binop.sign = "PLUS";

            if(NOW_ARRAY->kind == BASIC){
                ARRAY_LEVEL = 0;
            }

            if(!(p2->kind == BASIC && p2->u.basic == TYPE_INT)){
                // error 12
                error(12, node->line, "Array access should be a integer");
                return DEFAULT_TYPE;
            }
            if(p1->kind != ARRAY){
                //error 10
                assert(node!=NULL);
                error(10, node->line, "this is not a Array");
                return DEFAULT_TYPE;
            }
            retType = (Type)malloc(sizeof(struct Type_));
            memcpy(retType, p1->u.array.elem, sizeof(struct Type_));
            retType->assign = LEFT;
            return retType;
        }else if(!strcmp(node->name, "DOT")){
            //Exp -> Exp DOT ID
            Type p1 = Exp(defaultPlace, fstChild);
            if(p1->kind != STRUCTURE){
                //error 13
                error(13, node->line, "this is not a struct");
                return DEFAULT_TYPE;
            }
            FieldList p = p1->u.structure->fieldList;
            int flag = 0;
            while(p != NULL){
                if(!strcmp(p->name, node->broTree->idType)){
                    flag = 1;
                    break;
                }
                p = p->tail;
            }
            if(!flag){
                //error 14
                error(14, node->line, infoCat("Non-existent field :",node->broTree->idType));
                return DEFAULT_TYPE;
            }

            retType = (Type)malloc(sizeof(struct Type_));
            memcpy(retType, p->type,sizeof(struct Type_));
            retType->assign = LEFT;
            return retType;
        }
    }else if(!strcmp(node->name, "NOT")){
        //Exp -> NOT Exp
        Type p = Exp(defaultPlace, node->broTree);
        if(!(p->kind == BASIC && p->u.basic == TYPE_INT)){
            //error 7
            error(7, node->line, "Type mismatched for operands, Only int");
            return DEFAULT_TYPE;
        }
        retType = (Type)malloc(sizeof(struct Type_));
        memcpy(retType, p, sizeof(struct Type_));
        retType->assign = RIGHT;
        return retType;
    }else if(!strcmp(node->name, "MINUS")){
        //Exp -> MinUS Exp

        //intercode
        Operand t = createOperand();
        t->kind = VAR;
        t->u.value = new_temp();
        Type p = Exp(t, node->broTree);
        InterCode minusCode = createInterCode();
        minusCode->kind = OPERATION;
        minusCode->u.binop.sign = node->name;
        Operand t1 = createOperand();
        t1->kind = CONSTANT_INT;
        t1->u.intVar = 0;
        minusCode->u.binop.op1 = t1;
        minusCode->u.binop.op2 = t;
        minusCode->u.binop.result = place;


        if(p->kind == BASIC){
            retType = (Type)malloc(sizeof(struct Type_));
            memcpy(retType, p, sizeof(struct Type_));
            retType->assign = RIGHT;
            return retType;
        }else{
            //error 7
            error(7, node->line, "Type mismatched for operands, Only int or float");
            return DEFAULT_TYPE;
        }
    }else if(!strcmp(node->name, "LP")){
        return Exp(defaultPlace, node->broTree);
    }

    // printf("Exp error \n");
}

//finished lab3 finished
int Args(struct TreeNode* node, FieldList param){
    debug(node, "Args");
    if(param == NULL){
        return 0;
    }

    node = node->subTree;


    //intercode
    Operand t = createOperand();
    t->kind = VAR;
    t->u.value = new_temp();
    Type type = Exp(t, node);
    InterCode argCode = (InterCode)malloc(sizeof(struct InterCode_));
    if(type->kind == ARRAY){
        t->kind = VAR2ADDR;
    }
    argCode->kind = ARG_C;
    argCode->u.sinop.op = t;
    sizeOfCodes++;
    argCodes[sizeOfCodes - 1] = argCode;

    if(!typeEqual(param->type, type)){
        return 0;
    }
    if(node->broTree != NULL){
        //Args -> Exp , Args
        return Args(node->broTree->broTree, param->tail);
    }else{
        if(param->tail != NULL)
            return 0;
    }
    return 1;
}

char* new_temp(){
    T = T + 1;
    char* v = (char*)malloc(LEN);
    memset(v, 0, LEN);
    strcat(v, "t");
    char num[LEN - 1];
    sprintf(num, "%d",T);
    strcat(v, num);
    return v;
}

char* new_var(){
    V = V + 1;
    char* v = (char*)malloc(LEN);
    memset(v, 0, LEN);
    strcat(v, "v");
    char num[LEN-1];
    sprintf(num, "%d",V);
    strcat(v, num);
    return v;
}

char* new_lab(){
    L = L + 1;
    char* v = (char*)malloc(LEN + 5);
    memset(v, 0, LEN + 5);
    strcat(v, "v");
    char num[LEN + 5 - 1];
    sprintf(num, "%d",V);
    strcat(v, num);
    return v;
}

//lab3 ing
int isParam(char* name){

    return 0;
}