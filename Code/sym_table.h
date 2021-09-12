#ifndef SYM_TABLE_H
#define SYM_TABLE_H
#include "syntax_tree.h"
//VARIABLE
typedef struct FUNCTIONLIST
{
    char* name;
    struct FUNCTIONLIST* next;
}func_list;
typedef struct FieldList_ FieldList;
typedef struct Type_
{
    enum
    {
        BASIC,
        ARRAY,
        STRUCTURE
    } kind;
    union
    {
        int basic; //0:INT 1:FLOAT
        struct
        {
            struct Type_ *elem;
            int size;
        } array;                      //array
        struct FieldList_ *structure; //structue
    } u;
    bool r_value;   //if this is a right value
} Type;

struct FieldList_
{
    char *name;
    Type *type;
    FieldList *next; //next field
};

//FUNCTION
struct t_paralist
{
    Type *para_type;
    struct t_paralist *next; //参数类型的列表
};
typedef struct func_type
{
    struct t_paralist *para_list;
    char *name;
    bool defined;
    //int sort; //0:declare 1:define
    int dim;
    int line;
    Type *r_type; //返回值类型
    struct func_type *next;
} F_Type;

//SYM_TABLE
typedef struct sym_table
{
    Type *type;
    char *name;
    struct sym_table *next;
} Sym_Type;

//STRUCT_TABLE
typedef struct st_table
{
    Type *type;
    struct st_table *next;
} St_Type;

//High-level Definitions
void Program(Node *root);
void ExtDefList(Node *root);
void ExtDef(Node *root);
void ExtDecList(Node *root, Type *type);

//Specifiers
Type *Specifier(Node *root);
Type *StructSpecifier(Node *root);
char *OptTag(Node *root);
char *Tag(Node *root);

//Declarators
Type* VarDec(Node *root, Type *type, FieldList* fieldsTail, bool isDef);
void FunDec(Node *root, Type *r_type, bool isDef);
Type *ParamDec(Node *root,bool isDef);
struct t_paralist *VarList(Node *root, int* dim, bool isDef);

//Statements
void CompSt(Node *root, Type *r_type);
void StmtList(Node *root, Type *r_type);
void Stmt(Node *root, Type *r_type);

//Local Definitions
void DefList(Node *root, FieldList* fieldsTail);
void Def(Node *root, FieldList* fieldsTail);
void DecList(Node *root, Type *type, FieldList* fieldsTail);
void Dec(Node *root, Type *type, FieldList* fieldsTail);

//Expressions
Type *Exp(Node *root); //every expression has a type
void Args(Node *root, struct t_paralist* para_list); //parameter_list, check each parameter's type

//Error
void print_error(int errorNo,int line, char* name);
void check_func_def();

#endif