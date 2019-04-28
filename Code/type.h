#ifndef TYPE_H
#define TYPE_H
/* 
* type define and equal
*/
#define TYPE_INT 1
#define TYPE_FLOAT 2

typedef struct Type_* Type;
typedef struct Structure_* Structure;
typedef struct Funtion_* Funtion;
typedef struct FieldList_* FieldList;

struct Type_* DEFAULT_TYPE;

void initialType();
char* getFuncInf(Funtion src);
int typeEqual(Type src, Type dst);



struct Type_{
    enum { BASIC, ARRAY, STRUCTURE, FUNTION, NULL_TYPE } kind;
    union{
        // 基本类型
        int basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct { Type elem; int size; } array;
        // 结构体类型信息是一个链表
        Structure structure;
        //funtion
        Funtion funtion;
    } u;
    enum{LEFT, RIGHT} assign;
};

struct Structure_{
    char* name;
    FieldList fieldList;
};

struct Funtion_{
    char* name;
    Type retType;
    FieldList paras;
};

struct FieldList_{
    char* name; // 域的名字
    Type type; // 域的类型
    FieldList tail; // 下一个域
};

#endif