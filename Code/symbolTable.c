#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "type.h"
#include "symbolTable.h"

// #define DEBUG

Term SymbolTable[SIZE];

unsigned int hash_pjw(char* name){
    unsigned int val = 0, i;
    for (; *name; ++name){
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val % SIZE;
}

Type getTable(char* name){
    unsigned int val = hash_pjw(name);
#ifdef DEBUG
    printf("get %d %s type\n", val, name);
#endif
    Term p = SymbolTable[val];
    while(p != NULL){
        if(!strcmp(name, p->name)){
            return p->type;
        }
        p = p->next;
    }
    return NULL;
}

int checkNotExist(char* name){
    unsigned int val = hash_pjw(name);
    Term p = SymbolTable[val];
    while(p != NULL){
        if(!strcmp(name, p->name)){
            return 0;
        }
        p = p->next;
    }
    return 1;
}

int structInsertTable(Type type){
    //what inserted must be named and inserted firstly
    unsigned int val = hash_pjw(type->u.structure->name);
    Term p = (Term)malloc(sizeof(struct Term_));
    p->name = type->u.structure->name;
    p->type = type;
    p->next = SymbolTable[val];
    SymbolTable[val] = p;
#ifdef DEBUG
    printf("insert struct name:%s \n", type->u.structure->name);
#endif
    return 1;
}

int varInsertTable(char* name, Type type){
    //cha ru bian liang
    unsigned int val = hash_pjw(name);
    Term p = (Term)malloc(sizeof(struct Term_));
    p->name = name;
    p->type = type;
    p->next = SymbolTable[val];
    SymbolTable[val] = p;
#ifdef DEBUG
    printf("insert var %d name:%s \n", val, name);
#endif
    return 1;
}

void initSymbolTable(){
    const char* write = "write";
    Type writeType = (Type)malloc(sizeof(struct Type_));
    writeType->kind = FUNTION;

    writeType->u.funtion = (Funtion)malloc(sizeof(struct Funtion_));
    writeType->u.funtion->name = write;
    writeType->u.funtion->paras = (FieldList)malloc(sizeof(struct FieldList_));
    writeType->u.funtion->paras->type = (Type)malloc(sizeof(struct Type_));
    writeType->u.funtion->paras->type->kind = BASIC;
    writeType->u.funtion->paras->type->u.basic = TYPE_INT;
    writeType->u.funtion->paras->tail = NULL;

    writeType->u.funtion->retType = (Type)malloc(sizeof(struct Type_));
    writeType->u.funtion->retType->kind = BASIC;
    writeType->u.funtion->retType->u.basic = TYPE_INT;

    varInsertTable(write, writeType);

    const char* read = "read";
    Type readType = (Type)malloc(sizeof(struct Type_));
    readType->kind = FUNTION;
    readType->u.funtion = (Funtion)malloc(sizeof(struct Funtion_));
    readType->u.funtion->name = read;

    readType->u.funtion->retType = (Type)malloc(sizeof(struct Type_));
    readType->u.funtion->retType->kind = BASIC;
    readType->u.funtion->retType->u.basic = TYPE_INT;

    varInsertTable(read, readType);
}