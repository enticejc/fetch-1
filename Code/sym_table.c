#include "sym_table.h"
#include "assert.h"
//sym,func and struct table
St_Type *struct_table[0x3fff];
Sym_Type *symbol_table[0x3fff];
F_Type *function_table[0x3fff];
func_list* function_list;
FieldList* present_field;
//封装一下strcmp，逻辑上直观点
bool isToken(Node* node,const char* tokenName){
    assert(node);
    return !strcmp(node->name,tokenName);
}

bool isINT(Type* type){
    return type->kind==BASIC && type->u.basic==0;
}

bool isFLOAT(Type* type){
    return type->kind==BASIC && type->u.basic==1;
}
//避免到处return NULL，容易segfault，发现语义错误时随便创建一个basic type返回
Type* makeBasicType(bool isFloat){
    Type* type = (Type*)malloc(sizeof(Type));
    type->kind = BASIC;
    type->r_value = false;
    type->u.basic = isFloat;
    return type;
}

//创建域节点
FieldList* makeFieldList(Type* type, char* name){
    FieldList* ret = (FieldList*) malloc (sizeof(FieldList));
    ret->name = name;
    ret->type = type;
    ret->next = NULL;
    return ret;
}

//匿名函数命名
int name_count = 0;
char* creatName(){
    char* name = (char*)malloc(sizeof(char)*(2+name_count));
    sprintf(name,"%d",name_count++);
    return name;
}

//insert and find
F_Type *find_Func(char *name)
{
    //fprintf(stderr,"find_Func:name=%s, ",name);
    int sum = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        sum += name[i];
    }
    for (F_Type *p = function_table[sum % 0x3fff]; p != NULL; p = p->next)
    {
        if ((!strcmp(name, p->name))){
            //fprintf(stderr, "found.\n");
            return p;
        }
            
    }
    //fprintf(stderr, "failed.\n");
    return NULL;
}
void check_func_def()//deal with error type 18
{
    func_list* node=function_list;
    while(node !=NULL)
    {
        F_Type* func_1=find_Func(node->name);
        if(!func_1->defined) print_error(18,func_1->line,node->name);
        node=node->next;
    }
}
void insert_into_func_list(char*name)
{
    func_list* temp=(func_list*)malloc(sizeof(func_list));
    temp->next=function_list;
    function_list=temp;
    temp->name=name;
}
Sym_Type *find_Sym(char *name)
{
    //fprintf(stderr,"find_Sym:name=%s, ",name);
    int sum = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        sum += name[i];
    }
    for (Sym_Type *p = symbol_table[sum % 0x3fff]; p != NULL; p = p->next)
    {
        if (!strcmp(name, p->name)){
            //fprintf(stderr, "found.\n");
            return p;
        }
    }
    //fprintf(stderr, "failed.\n");
    return NULL;
}

St_Type *find_Stru(char *name)
{
    //fprintf(stderr,"find_Stru:name=%s, ",name);
    int sum = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        sum += name[i];
    }
    for (St_Type *p = struct_table[sum % 0x3fff]; p != NULL; p = p->next)
    {
        if (!strcmp(name, p->type->u.structure->name)){
            //fprintf(stderr, "found.\n");
            return p;
        }
    }
    //fprintf(stderr, "failed.\n");
    return NULL;
}

void insert_Func(char *name, bool defined, int dim, Type *r_type, struct t_paralist *para_list, int line)
{
    //fprintf(stderr,"Insert_Func:r_type=%d, name=%s, defined=%d\n",r_type->kind,name,defined);
    int sum = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        sum += name[i];
    }
    F_Type *p = (F_Type *)malloc(sizeof(F_Type));
    p->next = function_table[sum % 0x3fff];
    function_table[sum % 0x3fff] = p;
    p->dim = dim;
    p->name = name;
    p->para_list = para_list;
    p->r_type = r_type;
    p->defined = defined;
    p->line = line;
    insert_into_func_list(name);
}

bool type_check(Type *a, Type *b)
{
    if (a == NULL || b == NULL)
        return false;
    if (a->kind != b->kind)
        return false;
    if (a->kind == BASIC)
    {
        if (a->u.basic != b->u.basic)
            return false;
    }
    else if (a->kind == ARRAY)
    {
        return type_check(a->u.array.elem, b->u.array.elem);
    }
    else if (a->kind == STRUCTURE)
    {
        if (strcmp(a->u.structure->name, b->u.structure->name))
            return false;
    }
    return true;
}

bool check_func(int dim, Type *r_type, struct t_paralist *para_list, F_Type *p)
{
    if (p->dim != dim)
        return false;
    if (!type_check(r_type, p->r_type))
        return false;
    for (struct t_paralist *i = para_list, *j = p->para_list; i != NULL; i = i->next, j = j->next)
    {
        if (!type_check(i->para_type, j->para_type))
            return false;
    }
    return true;
}

void defFunc(F_Type *p)
{
    p->defined = 1;
}

void insert_Struc(Type *type)
{
    char* name = type->u.structure->name;
    //fprintf(stderr,"Insert_Struc:type=%d, name=%s\n",type->kind,name);
    int sum = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        sum += name[i];
    }
    St_Type *p = (St_Type *)malloc(sizeof(St_Type));
    p->type = type;
    p->next = struct_table[sum % 0x3fff];
    struct_table[sum % 0x3fff] = p;
}

void insert_Sym(Type *type, char *name)
{
    //fprintf(stderr,"Insert_Sym:type=%d, name=%s\n",type->kind,name);
    int sum = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        sum += name[i];
    }
    Sym_Type *p = (Sym_Type *)malloc(sizeof(Sym_Type));
    p->name = name;
    p->type = type;
    p->next = symbol_table[sum % 0x3fff];
    symbol_table[sum % 0x3fff] = p;
}


void Program(Node *root)
{
    if (!root || root->vacuum) return;
    assert(isToken(root, "Program"));
    //printf("Program\n");
    ExtDefList(root->child);
}

void ExtDefList(Node *root){
    if (!root || root->vacuum) return;
    assert(isToken(root, "ExtDefList"));
    //printf("ExtDefList\n");
    Node* def = root->child;
    ExtDef(def);
    ExtDefList(def->next);
}

void ExtDef(Node *root){
    if (!root || root->vacuum) return;
    assert(isToken(root, "ExtDef"));
    //printf("ExtDef\n");
    Type* type = Specifier(root->child);
    Node* next = root->child->next;
    assert(next && !next->vacuum);
    //ExtDef -> Specifier SEMI
    if (isToken(next,"SEMI")){
        insert_Sym(type, creatName());
        return;
    }
    if (isToken(next,"ExtDecList")){
        ExtDecList(next,type);
    }
    if (isToken(next,"FunDec")){
        if (isToken(next->next,"CompSt")){
            FunDec(next,type,true); //func definition
            CompSt(next->next,type);
        }
        else
            FunDec(next,type,false);  //func declaration
    }

}

void ExtDecList(Node *root, Type *type){
    if (!root || root->vacuum) return;
    assert(isToken(root, "ExtDecList"));
    //printf("ExtDecList\n");
    Node* var = root->child;
    VarDec(var,type, NULL,true);

    //ExtDecList -> VarDec COMMMA ExtDecList
    if (var->next)
        ExtDecList(var->next->next,type);
}

//Specifiers
Type *Specifier(Node *root){
    assert(root && !root->vacuum);
    assert(isToken(root, "Specifier"));
    //printf("Specifier\n");

    Node* typeNode = root->child;
    //Specifier -> TYPE
    if (isToken(typeNode,"TYPE")){
        bool isFloat = strcmp(typeNode->val,"int");
        Type* t = makeBasicType(isFloat);
        t->r_value = false;
        return t;
    }else{  //Specifier -> StructSpecifier
        return StructSpecifier(typeNode);
    }

}

Type *StructSpecifier(Node *root){
    assert(root && !root->vacuum);
    assert(isToken(root, "StructSpecifier"));
    //printf("StructSpecifier\n");
    
    Node* tag = root->child->next;

    if (isToken(tag,"Tag")){
        char* name = Tag(tag);
        St_Type *st_type = find_Stru(name);
        //17:使用未定义的结构体
        if (!st_type){
            print_error(17,root->line,name);
            return makeBasicType(0);
        }
        return st_type->type;
    }
    char* name = OptTag(tag);
    //16:结构体与变量或其他结构体重名
    if (find_Stru(name) || find_Sym(name)){
        print_error(16,root->line,name);
        return makeBasicType(0);
    }
    Node* defList = tag->next->next;
    Type* type = (Type*)malloc(sizeof(Type));
    type->kind = STRUCTURE;
    type->u.structure = (FieldList*) malloc(sizeof(FieldList));
    type->u.structure->name = name;
    present_field=type->u.structure;
    DefList(defList,type->u.structure);
    insert_Struc(type);

    return type;
}

char *OptTag(Node *root){
    assert(root && isToken(root,"OptTag"));
    //printf("OptTag\n");
    if (root->vacuum){
        return creatName();
    }
    Node* idNode = root->child;
    return idNode->val;
}
char *Tag(Node *root){
    assert(root && !root->vacuum && isToken(root,"Tag"));
    //printf("Tag\n");
    Node* idNode = root->child;
    return idNode->val;
}

//Declarators
Type* VarDec(Node *root, Type *type, FieldList* fieldsTail, bool isDef){
    if (!root || root->vacuum) return makeBasicType(0);
    assert(isToken(root,"VarDec"));
    //printf("VarDec\n");
    Node* first = root->child;
    //VarDec -> ID
    if (isToken(first,"ID")){
        char* varName = first->val;
        bool field_check=true;
        if(fieldsTail){
            for(FieldList* p=present_field->next;p!=NULL;p=p->next){
                if(!strcmp(p->name,varName)){
                    field_check=false;
                    break;
                }
            }
        }
        if(!field_check){
            print_error(15, root->line, varName);
        }
        else if ((find_Sym(varName) || find_Stru(varName))&&isDef){//declare no need to report the error
            print_error(3, root->line, varName);
        }else{
            if(isDef)
            insert_Sym(type, varName);
            if (fieldsTail){//STRUCTURE Def
                fieldsTail->next = makeFieldList(type,varName);
            }
        }
        return type;
    }else{  //first->name == "VarDec"
    //VarDec -> VarDec LB INT RB	
        Node* sizeNode = first->next->next;
        assert(isToken(sizeNode,"INT"));
        int size = atoi(sizeNode->val);
        Type* type1 = (Type*)malloc(sizeof(Type));
        type1->kind = ARRAY;
        type1->r_value = false;
        type1->u.array.elem = type;
        type1->u.array.size = size;
        return VarDec(first,type1, fieldsTail,isDef);
    }
}

void FunDec(Node *root, Type *r_type, bool isDef){
    if (!root || root->vacuum) return;
    assert(isToken(root, "FunDec"));
    //printf("FunDec\n");
    Node* idNode = root->child;
    char* fName = idNode->val;
    Node* varList = idNode->next->next;
    struct t_paralist* para_list = NULL;

    int* dim = (int*) malloc(sizeof(int));
    *dim = 0;
    if (isToken(varList,"VarList")){
        para_list = VarList(varList, dim, isDef);//TODO: 这里需要增加根据isDef处理
    }
    int d = *dim;
    free(dim);
    F_Type* f = find_Func(fName);
    if (!f){
        insert_Func(fName,isDef,d,r_type,para_list,root->line);
    }else/* already defined or declared */{if (isDef){
            if (f->defined){//multiple defined
                print_error(4,root->line, fName);
            }else{
                if (!check_func(d,r_type,para_list,f)){//type un-consisted
            print_error(19,root->line, fName);
            }
            else f->defined = true;
            }
        }
        else{
            if (!check_func(d,r_type,para_list,f)){//type un-consisted
            print_error(19,root->line, fName);
            }
        }
    }
}

struct t_paralist *VarList(Node *root, int* dim, bool isDef){
    assert(root && !root->vacuum);
    assert(isToken(root, "VarList"));
    //printf("VarList\n");
    Node* first = root->child;
    Type* type = ParamDec(first,isDef);
    (*dim)++;
    struct t_paralist* para_list = (struct t_paralist*) malloc(sizeof(struct t_paralist));
    para_list->para_type = type;
    
    if (!first->next){
        para_list->next = NULL;
    }else{
        para_list->next = VarList(first->next->next,dim,isDef);
    }
    return para_list;
}

Type *ParamDec(Node *root,bool isDef){
    assert(root && !root->vacuum);
    assert(isToken(root, "ParamDec"));
    //printf("ParamDec\n");
    Node* typeNode = root->child;
    assert(isToken(typeNode,"Specifier"));
    Type* type = Specifier(typeNode);
    Node* varNode = typeNode->next;
    Type* type_=VarDec(varNode,type, NULL, isDef);
    return type_;
}

//Statements
void CompSt(Node *root, Type *r_type){
    assert(root && !root->vacuum);
    assert(isToken(root, "CompSt"));
    //printf("CompSt\n");
    Node* defList = root->child->next;
    DefList(defList, NULL);//no field list here
    Node* stmtList = defList->next;
    StmtList(stmtList, r_type);
}

void StmtList(Node *root, Type *r_type){
    if (!root || root->vacuum) return;
    assert(isToken(root, "StmtList"));
    //printf("StmtList\n");
    Node* stmt = root->child;
    Stmt(stmt, r_type);
    StmtList(stmt->next,r_type);
}

void Stmt(Node *root, Type *r_type){
    if (!root || root->vacuum) return;
    assert(isToken(root, "Stmt"));
    //printf("Stmt\n");
    Node* first = root->child;
    if (isToken(first, "Exp")){
        Exp(first);return;
    }
    if (isToken(first, "CompSt")){
        CompSt(first,r_type);return;
    }
    if (isToken(first, "RETURN")){
        Type* type = Exp(first->next);
        if (!type_check(type, r_type)){
            print_error(8,root->line,NULL);
        }
        return;
    }
    if (isToken(first, "IF")){
        Node* exp = first->next->next;
        Exp(exp);
        Node* stmt1 = exp->next->next;
        Stmt(stmt1, r_type);
        if (stmt1->next){
            Node* stmt2 = stmt1->next->next;
            Stmt(stmt2, r_type);
        }
        return;
    }
    if (isToken(first,"WHILE")){
        Node* exp = first->next->next;
        Exp(exp);
        Node* stmt1 = exp->next->next;
        Stmt(stmt1, r_type);
        return;
    }
}

//Local Definitions
void DefList(Node *root, FieldList* fieldsTail){
    if (!root || root->vacuum) return;
    assert(isToken(root, "DefList"));
    //printf("DefList\n");
    Node* def = root->child;
    Def(def, fieldsTail);
    while (fieldsTail && fieldsTail->next){
        fieldsTail = fieldsTail->next;
    }
    DefList(def->next, fieldsTail);
}
void Def(Node *root, FieldList* fieldsTail){
    assert(root && !root->vacuum);
    assert(isToken(root, "Def"));
    //printf("Def\n");
    Node* typeNode = root->child;
    assert(isToken(typeNode,"Specifier"));
    Type* type = Specifier(typeNode);
    DecList(typeNode->next, type, fieldsTail);
}
void DecList(Node *root, Type *type, FieldList* fieldsTail){
    assert(root && !root->vacuum);
    assert(isToken(root, "DecList"));
    //printf("DecList\n");
    Node* dec = root->child;
    Dec(dec,type, fieldsTail);
    if(fieldsTail){
        fieldsTail = fieldsTail->next;
    } 
    if (dec->next){
        DecList(dec->next->next, type, fieldsTail);
    }
}
void Dec(Node *root, Type *type, FieldList* fieldsTail){
    assert(root && !root->vacuum);
    assert(isToken(root, "Dec"));
    //printf("Dec\n");
    Node* var = root->child;
    VarDec(var,type, fieldsTail,true);
    
    if (var->next){
        Node* op = var->next;
        assert(isToken(op,"ASSIGNOP"));
        if(fieldsTail) print_error(15,root->line,"");
        Type* r_type = Exp(op->next);
        type_check(type,r_type);
    }
}

//Expressions
//every expression has a type
Type *Exp(Node *root){
    assert(root && !root->vacuum);
    assert(isToken(root, "Exp"));
    //printf("Exp\n");
    Node* first = root->child;
    //printf("first->name,%s\n",first->name);
    if (isToken(first,"ID")){ 
        //Exp->ID|ID LP Args RP|ID LP RP
        if (!first->next){  //ID is an variable
            char* vname = first->val;
            Sym_Type* sym = find_Sym(vname);
            if (!sym){
                print_error(1, root->line, vname);
                return makeBasicType(0);
            }
            return sym->type;
        }
        //ID should be a function
        char* fname = first->val;
        F_Type* func = find_Func(fname);

        if (!func && (find_Stru(fname) || find_Sym(fname))){
            print_error(11, root->line, fname);
            return makeBasicType(0);
        }
        if (!func || !func->defined){
            print_error(2, root->line, fname);
            return makeBasicType(1);
        }
        Node* args = first->next->next;
        if (isToken(args,"Args")){//对比形参表
            Args(args,func->para_list);
        }else{  //应传参，未传
            if (func->dim>0){
                print_error(9, root->line, NULL);
            }
        }
        return func->r_type;
    }
    //注意右值
    if (isToken(first,"INT")){
        Type* type = makeBasicType(0);
        type->r_value = true;
        return type;
    }
    if (isToken(first,"FLOAT")){
        Type* type = makeBasicType(1);
        type->r_value = true;
        return type;
    }
    if (isToken(first,"LP")){
        return Exp(first->next);
    }
    //只有INT可以进行取非
    if (isToken(first,"NOT")){
        Type* type = Exp(first->next);
        if (!isINT(type)){
            print_error(7,root->line, NULL);
        }
        return type;
    }
    //只有INT和FLOAT可以取负
    if (isToken(first,"MINUS")){
        Type* type = Exp(first->next);
        if (!(type->kind==BASIC)){
            print_error(7,root->line, NULL);
        }
        return type;
    }

    if (isToken(first,"Exp")){
        Type* t1 = Exp(first);
        if(!t1)
        {
            printf("error line:%d   \"%s\"\n",root->line,first->child->child->val);
            fflush(stdout);
        }
        assert(t1);
        Node* optr = first->next;
        //结构体
        if (isToken(optr,"DOT")){
            //fprintf(stderr, "IN DOT\n");
            if (t1->kind!=STRUCTURE){
                print_error(13,root->line, NULL);
                return t1;
            }
            Node* idNode = optr->next;
            assert(t1->u.structure && isToken(idNode,"ID"));
            FieldList* field = t1->u.structure->next;
            //fprintf(stderr, "Before loop, id=%s\n",idNode->val);
            while (field && field->name){
                //fprintf(stderr, "IN loop, field->name=%s\n", field->name);
                if (!strcmp(idNode->val, field->name)) 
                    return field->type;
                field = field->next;
            }
            //fprintf(stderr, "after loop\n");
            print_error(14, root->line, idNode->val);
            return t1;
        }
        //数组
        if (isToken(optr,"LB")){
            if (t1->kind!=ARRAY){
                print_error(10,root->line, NULL);
                return t1;
            }
            Node* exp = optr->next;
            Type* idx = Exp(exp);
            if (!isINT(idx)){
                print_error(12,root->line, NULL);
            }
            return t1->u.array.elem;
        }
        //以下为二元运算
        //右值不可在赋值符左边
        if (isToken(optr,"ASSIGNOP") && t1->r_value){
            print_error(6, root->line, NULL);
            return t1;
        }
        //只有INT以进行逻辑运算
        if (!isINT(t1) && (isToken(optr,"AND") || isToken(optr,"OR"))){
            print_error(7,root->line, NULL);
            return t1;
        }
        //只有INT和FLOAT可以进行加减乘除和比较
        if (!(t1->kind==BASIC) && 
        (isToken(optr,"RELOP") 
        || isToken(optr,"PLUS") || isToken(optr,"MINUS") 
        || isToken(optr,"STAR") || isToken(optr,"DIV") )){ 
            print_error(7,root->line, NULL);
            return t1;
        }
        //赋值符两边类型应匹配
        Type* t2 = Exp(optr->next);
        if (isToken(optr,"ASSIGNOP") && !type_check(t1,t2)){
            print_error(5, root->line, NULL);
            return t1;
        }
        //运算符两边类型应匹配
        if (!type_check(t1,t2)){
            print_error(7,root->line, NULL);
            return t1;
        }
        return t1;
    }
} 

//parameter_list, check each parameter's type
void Args(Node *root, struct t_paralist* para_list){
    assert(root && !root->vacuum);
    assert(isToken(root, "Args"));
    //printf("Args\n");
    Node* first = root->child;
    Type* type = Exp(first);
    if (!para_list || !type_check(type,para_list->para_type)){
        print_error(9,root->line,NULL);
        return;
    }
    if (first->next){
        Args(first->next->next, para_list->next);
    }
} 


void print_error(int errorNo,int line, char* name){
    printf("ERROR TYPE %d at line %d: ", errorNo, line);
    switch (errorNo)
    {
    case 1:
        printf("Undefined variable \"%s\".\n", name);
        break;
    case 2:
        printf("Undefined function \"%s\".\n", name);
        break;
    case 3:
        printf("Redefined variable \"%s\".\n", name);
        break;
    case 4:
        printf("Redefined function \"%s\".\n", name);
        break;
    case 5:
        printf("Type mismatched for assignment.\n");
        break;
    case 6:
        printf("The left-hand side of an assignment must be a variable.\n");
        break;
    case 7:
        printf("Type mismatched for operands.\n");
        break;
    case 8:
        printf("Type mismatched for return.\n");
        break;
    case 9:
        printf("Function is not applicable for arguments.\n");//报错信息可扩充
        break;
    case 10:
        printf("The variable is not an array.\n");//报错信息可扩充
        break;
    case 11:
        printf("The variable is not a function.\n");//报错信息可扩充
        break;
    case 12:
        printf("The array index should be an integer.\n");//报错信息可扩充
        break;
    case 13:
        printf("Illegal use of \".\".\n");
        break;
    case 14:  
        printf("Non-existent field \"%s\".\n", name);
        break;
    case 15:  
        printf("Redefined or initialed field.\n");
        break;
    case 16:  
        printf("Duplicated name \"%s\".\n", name);
        break;
    case 17:  
        printf("Undefined structure \"%s\".\n", name);
        break;  
    case 18:    //TODO: impossible now, 需要实现在语义分析完成后遍历所有函数进行检查的功能
        printf("Undefined function \"%s\".\n", name);
        break; 
    case 19:
        printf("Inconsistent declaration of function \"%s\".\n", name);
        break;   
    default:
        printf("UNDEFINED ERROR NUMBER.\n");
        break;
    }
    
}
