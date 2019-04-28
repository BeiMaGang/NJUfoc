#include <string.h>
#include "type.h"


char rnt[1024];
void initialType(){
    DEFAULT_TYPE = (Type)malloc(sizeof(struct Type_));
    DEFAULT_TYPE->kind = NULL_TYPE;
}

int typeEqual(Type src, Type dst){
    if(src->kind != dst->kind)
        return 0;
    
    if(src->kind == BASIC && src->u.basic != dst->u.basic)
        return 0;

    if(src->kind == ARRAY){
        return typeEqual(src->u.array.elem, dst->u.array.elem);
    }

    if(src->kind == STRUCTURE){
        FieldList srcField = src->u.structure->fieldList;
        FieldList dstField = dst->u.structure->fieldList;
        while(srcField != NULL && dstField != NULL){
            if(!typeEqual(srcField->type, dstField->type)){
                return 0;
            }
            srcField = srcField->tail;
            dstField = dstField->tail;
        }
        if(srcField != NULL || dstField != NULL)
            return 0;
    }

    return 1;
}

char* getFuncInf(Funtion src){
    rnt[0] = '\0';
    strcat(rnt, src->name);
    strcat(rnt, "(");

    FieldList p = src->paras;
    while(p != NULL){
        if(p->type->kind == BASIC){
            if(p->type->u.basic == TYPE_INT){
                strcat(rnt, "int");
            }else
                strcat(rnt, "float");
        }else if(p->type->kind == ARRAY){
            strcat(rnt, "array");
        }else if(p->type->kind == STRUCTURE){
            strcat(rnt, p->type->u.structure->name);
        }
        p = p->tail;
        if(p!=NULL){
            strcat(rnt, ",");
        }
        
    }
    strcat(rnt, ")");
    return rnt;
}