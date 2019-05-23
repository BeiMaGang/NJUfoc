#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>//变长参数函数所需的头文件
#include"syntaxTree.h"

/*createNode()
name:文法的名字   eg:"program"
num: (箭头后面)语法单元的数量,num 
...:变长参数，就是num个语法单元的树节点
todo:此函数要做的事情就是生成一个树节点,兄弟节点不用动，子节点是传进来的第一个文法单元，第一个文法单元的兄弟节点一直往后.注意空指针
*/
struct TreeNode* createNode(char* name, int num, ...){
    va_list valist;
    struct TreeNode *father = (struct TreeNode*)malloc(sizeof(struct TreeNode)); //get father node
    struct TreeNode *temp = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    father->line = yylineno;
    if(!father){
        yyerror("out of space");
        exit(0);
    }

    father->name = name;  //set father
    va_start(valist,num);

    if(num > 0){
        temp = va_arg(valist, struct TreeNode*);
        father->subTree = temp;
        father->line = temp->line;

        if(num>=2){
            for(int i=0;i<num-1;i++){
                temp->broTree = va_arg(valist,struct TreeNode*);
                temp = temp->broTree;
            }
        }

    }else {
        if((!strcmp(father->name,"ID")) || (!strcmp(father->name,"TYPE"))){
            char *temp_name = (char*)malloc(sizeof(char*) * 40);
            strcpy(temp_name,yytext);
            father->idType = temp_name;
        }else if(!strcmp(father->name,"INT")){
            father->intType = atoi(yytext);
        }else if(!strcmp(father->name,"FLOAT")){
            father->floatType = atof(yytext);
        }else if(!strcmp(father->name,"RELOP")){
            char *temp_name = (char*)malloc(sizeof(char*) * 40);
            strcpy(temp_name,yytext);
            father->idType = temp_name;
        }
    }

    return father;

    // return (struct TreeNode* )malloc(sizeof(struct TreeNode));
}

//先序遍历
/*二叉树的先序遍历*/
void prePrint(struct TreeNode* parent, int level){

    if(parent != NULL){

        if(parent->flag != 1){
            for(int i=0; i<level; i++)
                printf("  ");
            printf("%s" , parent->name);
        }
        if(parent->subTree == NULL){
            if((!strcmp(parent->name,"ID")) || (!strcmp(parent->name,"TYPE"))){
                printf(": %s", parent->idType);
            }else if(!strcmp(parent->name,"INT")){
                printf(": %d", parent->intType);
            }else if(!strcmp(parent->name,"FLOAT")){
                printf(": %f", parent->floatType);
            }
        }else{
            printf(" (%d)", parent->line);
        }
        if(parent->flag != 1)
            printf("\n");
        
        prePrint(parent->subTree, level+1);
        prePrint(parent->broTree, level);
    }
}

void setFlag(struct TreeNode* node){
    node->flag = 1;
}