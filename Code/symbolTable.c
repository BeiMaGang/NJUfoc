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