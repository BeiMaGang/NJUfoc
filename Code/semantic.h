#ifndef SEMANTIC_H
#define SEMANTIC_H
/*
*semantic analysis process
*/

#include "symbolTable.h"
#include "syntaxTree.h"
#include "interCode.h"


void Program();
void ExtDefList(struct TreeNode* node);
void ExtDef(struct TreeNode* node);
void FunDec(struct TreeNode* node, Type type);
void ExtDecList(struct TreeNode* node, Type type);
void CompSt(struct TreeNode* node, Type returnType);
void StmtList(struct TreeNode* node, Type type);
void Stmt(struct TreeNode* node, Type type);

Type Specifier(struct TreeNode* node);
Type StructSpecifier(struct TreeNode* node);


FieldList VarList(struct TreeNode* node);
FieldList ParamDec(struct TreeNode* node);
FieldList DefList(struct TreeNode* node);
FieldList Def(struct TreeNode* node);
FieldList DecList(struct TreeNode* node,Type type);
FieldList Dec(struct TreeNode* node,Type type);
FieldList VarDec(struct TreeNode* node,Type type);


Type Exp(Operand place, struct TreeNode* node);
void Cond(Operand operand1, Operand operand2, struct TreeNode* node);

int Args(struct TreeNode* node, FieldList param);
#endif