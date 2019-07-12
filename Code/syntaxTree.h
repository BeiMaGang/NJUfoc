extern int yycolumn;
extern char* yytext;
struct TreeNode{
    int line;
    char* name;
    struct TreeNode* subTree;
    struct TreeNode* broTree;
    union{
        char* idType;
        int intType;
        float floatType;
    };
};

struct TreeNode* createNode(char* name, int num, ...);
void prePrint(struct TreeNode* parent, int level);