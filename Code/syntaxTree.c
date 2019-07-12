# include<stdio.h>
# include<stdlib.h>
# include<stdarg.h>//变长参数函数所需的头文件
# include"syntaxTree.h"

/*createNode()
name:文法的名字   eg:"program"
num: (箭头后面)语法单元的数量,num 
...:变长参数，就是num个语法单元的树节点
todo:此函数要做的事情就是生成一个树节点,兄弟节点不用动，子节点是传进来的第一个文法单元，第一个文法单元的兄弟节点一直往后.注意空指针
*/
struct TreeNode* createNode(char* name, int num, ...){
    return (struct TreeNode* )malloc(sizeof(struct TreeNode));
}

//先序遍历
/*二叉树的先序遍历*/
void prePrint(struct TreeNode* parent, int level){

}