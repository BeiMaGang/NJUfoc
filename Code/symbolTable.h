#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
/* 
*using Hash Table to implement
*/
#include "type.h"

#define SIZE 1024
typedef struct Term_* Term;

struct Term_ {
    char* name;
    Type type;
    Term next;
};

//symbol table
Term SymbolTable[SIZE];

//according to name to find Type
Type getTable(char* name);

int checkNotExist(char* name);

int structInsertTable(Type type);

// int funcInsertTable()

int varInsertTable(char* name, Type type);

#endif