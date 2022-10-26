#include "semantic.h"
#include "avl.h"
#include "node.h"

#define DEBUG

// Type* symbol_table[HASH_SIZE];
extern AVL_node *symbol_define_table;
extern AVL_node *symbol_announce_table; //符号表

Type *new_type(enum Kind kind)
{
    Type *p = (Type *)malloc(sizeof(Type));
    p->kind = kind;
    if (kind == BASIC)
    {
    }
    else if (kind == ARRAY)
    {
        p->array.elem = NULL;
        p->array.size = 0;
    }
    else if (kind == STRUCTURE)
    {
        p->structure = NULL;
    }
    else
    {
        p->func.para = NULL;
        p->func.para_num = 0;
    }
    return p;
}

Type *copy_type(Type *x)
{
    if (x == NULL)
        return NULL;
    Type *p = (Type *)malloc(sizeof(Type));
    p->kind = x->kind;
    if (p->kind == BASIC)
        p->basic = x->basic;
    else if (p->kind == ARRAY)
    {
        p->array.elem = copy_type(x->array.elem);
        p->array.size = x->array.size;
    }
    else if (p->kind == STRUCTURE)
    {
        ListNode *now = NULL;
        for (ListNode *i = x->structure; i; i = i->next)
        {

            ListNode *tmp = copy_listnode(i);
            if (p->structure == NULL)
            {
                p->structure = tmp;
                now = p->structure;
            }
            else
            {
                now->next = tmp;
                now = tmp;
            }
        }
    }
    else
    {
        p->func.para_num = x->func.para_num;
        ListNode *now = NULL;
        for (ListNode *i = x->func.para; i; i = i->next)
        {

            ListNode *tmp = copy_listnode(i);
            if (p->func.para == NULL)
            {
                p->func.para = tmp;
                now = p->func.para;
            }
            else
            {
                now->next = tmp;
                now = tmp;
            }
        }
    }
    //注意这些变量是联合类型，不要随便赋值，会被覆盖
    return p;
}

ListNode *copy_listnode(ListNode *x)
{
    ListNode *p = create_new_listnode();
    strcpy(p->name, x->name);
    p->type = copy_type(x->type);
}

ListNode *create_new_listnode()
{
    ListNode *p = (ListNode *)malloc(sizeof(ListNode));
    return p;
}

ListNode *new_listnode(char *name, Type *type)
{
    ListNode *p = create_new_listnode();
    strcpy(p->name, name);
    p->type = copy_type(type);
}

void deal_ExtDef(Node *k)
{
    // ExtDef = Specifier ExtDecList SEMI | Specifier SEMI |
    // Specifier FunDec CompSt
    Type *type = deal_Specifier(k->child);
    if (strcmp(k->child->next->name, "FunDec") == 0)
    {
        ListNode *fun = deal_FunDec(k->child->next, type);
        //注意，已添加，只能改变内部值
        // CompSt = LC DefList StmtList RC 函数具体定义，先声明后使用
        Node *compst = k->child->next->next;
        deal_CompSt(compst, type);
        //解析内部定义
    }
    else if (k->child_num == 3)
        deal_ExtDecList(k->child->next, type);
    return;
}

ListNode *deal_FunDec(Node *k, Type *type)
{
    // FunDec = ID LP VarList RP| ID LP RP //函数及参数列表
    if (!check_error4(k->child->val.char_val, k->child->line_num))
        return NULL;
    Type *funcself = new_type(FUNC); //函数类型
    ListNode *fun = new_listnode(k->child->val.char_val, funcself);
    fun->type->func.func_type = type; //返回值类型
    if (k->child_num == 4)
    { //有参数
        ListNode *now = deal_VarList(k->child->next->next);
        fun->type->func.para = now; //参数列表
    }
    insert_listnode(fun, &symbol_define_table); //添加自己
    return fun;
}

Type *deal_Specifier(Node *k)
{
    // Specifier = TYPE | StructSpecifier
    Type *now = NULL;
    if (strcmp(k->child->name, "TYPE") == 0) // int|float
    {

        now = new_type(BASIC);
        if (strcmp(k->child->val.char_val, "int") == 0)
            now->basic = Basic_int;
        else
            now->basic = Basic_float;
    }
    else if (strcmp(k->child->name, "StructSpecifier") == 0)
    {
        now = deal_StructSpecifier(k->child);
    }
    return now;
}

Type *deal_StructSpecifier(Node *k)
{
    // StructSpecifier = STRUCT OptTag LC DefList RC | STRUCT Tag
    Type *list = new_type(STRUCTURE); //结构体成员列表
    if (strcmp(k->child->next->name, "Tag") == 0)
    { // STRUCT Tag
        ListNode *tmp = search_listnode(symbol_define_table,
                                        k->child->next->child->val.char_val,
                                        STRUCTURE);
        if (tmp != NULL)
            list->structure = tmp->type->structure;
        //如果之前定义了就找到定义
        else
        {
            ListNode *p = new_listnode(k->child->next->child
                                           ->val.char_val,
                                       list);
            insert_listnode(p, &symbol_announce_table);
#ifdef DEBUG
            printf("STRUCT %s\n", p->name);
#endif
        }
    }
    else
    { // STRUCT OptTag LC DefList RC
        // OptTag = ID | empty
        if (k->child->next != NULL)
        { //有名字
            ListNode *p = new_listnode(k->child->next->child->val.char_val,
                                       list);
            insert_listnode(p, &symbol_define_table);
            if (k->child_num == 5)
            {
                ListNode *deflist = deal_DefList(k->child->next->next->next);
                list->structure = deflist;
            }
        }
        else
        { // STRUCT LC DefList RC
            if (k->child_num == 4)
            {
                ListNode *deflist = deal_DefList(k->child->next->next->next);
                list->structure = deflist;
            }
        }
    }
    return list;
}

ListNode *deal_DefList(Node *k)
{ //函数与结构体内部定义，返回参数列表
    // DefList = Def DefList | empty
    // Def = Specifier DecList SEMI//变量声明
    ListNode *head = NULL;
    ListNode *last = NULL;
    for (Node *i = k; i; i = i->child->next)
    {
        // DecList = Dec | Dec COMMA DecList
        Type *spe = deal_Specifier(i->child->child);
        //注意：函数参数类型可能是结构体，此时要找到之前的定义

        for (Node *j = i->child->child->next->child;;
             j = j->next->next->child) //每个变量的名称
        {
            ListNode *now = deal_Dec(j, spe);
            if (now != NULL)
            {
                insert_listnode(now, &symbol_define_table);
                ListNode *Var = copy_listnode(now);
                if (head == NULL)
                    head = last = Var;
                else
                {
                    last->next = Var;
                    last = Var;
                }
            }
            if (j->next == NULL)
                break;
        }
    }
    return head;
}

void deal_StmtList(Node *k, Type *t)
{ //函数内部使用
    // StmtList = Stmt StmtList | empty
    for (Node *i = k; i; i = i->child->next)
    {
        Node *stmt = i->child;
        check_stmt(stmt, t);
    }
}

ListNode *deal_CompSt(Node *compst, Type *t)
{
    // CompSt = LC DefList StmtList RC 函数具体定义，先声明后使用
    if (strcmp(compst->child->next->name, "DefList") == 0)
        deal_DefList(compst->child->next);
    if (strcmp(compst->child->next->name, "StmtList") == 0)
        deal_StmtList(compst->child->next, t);
    if (strcmp(compst->child->next->next->name, "StmtList") == 0)
        deal_StmtList(compst->child->next->next, t);
}

ListNode *deal_VarList(Node *k) //函数参数列表
{                               // VarList = ParamDec COMMA VarList| ParamDec
    // ParamDec = Specifier VarDec
    ListNode *last = NULL;
    ListNode *head = NULL;
    for (Node *i = k->child;; i = i->next->next->child)
    {
        //每一个参数定义
        Type *spe = deal_Specifier(i->child);
        ListNode *var = deal_VarDec(i->child->next, spe);
        if (var != NULL)
        {
            insert_listnode(var, &symbol_define_table);
            ListNode *Var = copy_listnode(var);
            if (head == NULL)
                head = last = Var;
            else
            {
                last->next = Var;
                last = Var;
            }
        }
        if (i->next == NULL)
            break;
    }
    return head;
}

void deal_ExtDecList(Node *k, Type *type)
{
    // ExtDecList = VarDec | VarDec COMMA ExtDecList | empty
    if (k == NULL)
        return;
    else
    {
        for (Node *i = k->child;; i = i->next->next->child)
        {
            //以逗号隔开的每个简单参数的头
            ListNode *now = deal_VarDec(i, type);
            if (now != NULL)
                insert_listnode(now, &symbol_define_table);
            if (i->next == NULL)
                break;
        }
    }
}

ListNode *deal_Dec(Node *k, Type *type)
{
    // Dec = VarDec | VarDec ASSIGNOP Exp
    ListNode *p = deal_VarDec(k->child, type);
    if (p == NULL)
        return NULL;
    if (k->child->next)
    {
    }
    //  insert_listnode(p);
    return copy_listnode(p);
}

ListNode *deal_VarDec(Node *k, Type *type)
{
    // VarDec = ID | VarDec LB INT RB
    if (strcmp(k->child->name, "ID") == 0)
    { // ID头
#ifdef DEBUG
        printf("ID : %s\n", k->child->val.char_val);
#endif
        if (!check_error3(k->child->val.char_val, k->child->line_num))
            return NULL;
        ListNode *now = new_listnode(k->child->val.char_val, type);
        return now; //数组
    }
    else
    {                                              //数组后半部分
        ListNode *p = deal_VarDec(k->child, type); //头节点，也就是TYPE
        if (p == NULL)
            return NULL;
        Type *tmp = p->type;
        while (tmp->kind == ARRAY)
            tmp = tmp->array.elem;       //最后一个数组维度
        Type *now = new_type(tmp->kind); //继承最开头的类型
        now->array.elem = NULL;
        tmp->array.size = k->child->next->next->val.int_val;
        //         printf("[%d]\n", now->array.size);
        tmp->array.elem = now;
        tmp->kind = ARRAY;
#ifdef DEBUG
        for (Type *i = p->type; i; i = i->array.elem)
            printf("%d  ", i->array.size);
        printf("\n");
#endif
        return p;
    }
}

void read(Node *k)
{
    if (k == NULL)
        return;
    //    printf("now read %s\n", k->name);
    if (strcmp(k->name, "ExtDef") == 0)
        deal_ExtDef(k);
    for (Node *i = k->child; i; i = i->next)
    {
        read(i);
    }
}

void insert_listnode(ListNode *k, AVL_node **table)
{
    if (if_exist(*table, k))
    {
        printf("%s already exist\n", k->name);
        return;
    }
    else
    {
        printf("%s doesn't exist\n", k->name);
        Insert(table, k);
        //     print_avl_tree(symbol_table);
    }
    return;
}

void check_stmt(Node *k, Type *t)
{
    // Stmt = Exp SEMI| CompSt| RETURN Exp SEMI | IF LP Exp RP Stmt
    //|IF LP Exp RP Stmt ELSE Stmt | WHILE LP Exp RP Stmt //使用
    if (k->child_num == 7)
    { // IF LP Exp RP Stmt ELSE Stmt
        Type *now = check_exp(k->child->next->next);
        check_stmt(k->child->next->next->next->next, t);
        check_stmt(k->child->next->next->next->next->next->next, t);
    }
    else if (k->child_num == 5)
    {
        // IF LP Exp RP Stmt | WHILE LP Exp RP Stmt
        check_exp(k->child->next->next);
        check_stmt(k->child->next->next->next->next, t);
    }
    else if (k->child_num == 3)
    {
        // RETURN Exp SEMI
        Type *now = check_exp(k->child->next);
        if (now)
        {
            if (!check_error8(now, t, k->child->line_num))
                return;
        }
    }
    else if (k->child_num == 2)
        // RETURN Exp SEMI
        check_exp(k->child);
    else
        deal_CompSt(k->child, t);
}

Type *check_exp(Node *k)
{ //返回的是运算结果类型
    // Exp = Exp ASSIGNOP Exp| Exp AND Exp | Exp OR Exp
    //| Exp RELOP Exp | Exp PLUS Exp | Exp MINUS Exp
    //| Exp STAR Exp | Exp DIV Exp | LP Exp RP | MINUS Exp
    //| NOT Exp | ID LP Args RP | ID LP RP | Exp LB Exp RB
    //| Exp DOT ID | ID | INT | FLOAT

    Type *now = NULL;
    if (k->child_num == 1)
    { //基本类型
        if (strcmp(k->child->name, "INT") == 0)
        {
            now = new_type(BASIC);
            now->basic = Basic_int;
        }
        else if (strcmp(k->child->name, "FLOAT") == 0)
        {
            now = new_type(BASIC);
            now->basic = Basic_float;
        }
        else if (strcmp(k->child->name, "ID") == 0)
        {
            if (!check_error1(k->child->val.char_val, k->child->line_num))
                return NULL;
            now = search_type(symbol_define_table, k->child->val.char_val);
        }
    }
    else if (k->child_num == 2) // NOT Exp | MINUS Exp
        check_exp(k->child->next);
    else if (k->child_num == 3)
    { // Exp ASSIGNOP Exp | Exp AND Exp | Exp OR Exp
        //| Exp RELOP Exp | Exp PLUS Exp | Exp MINUS Exp
        //| Exp STAR Exp | Exp DIV Exp | LP Exp RP
        //| ID LP RP | Exp DOT ID
        Type *tmp1 = NULL, *tmp2 = NULL, *tmp3 = NULL;
        if (strcmp(k->child->name, "Exp") == 0)
            tmp1 = check_exp(k->child);
        if (strcmp(k->child->next->name, "Exp") == 0)
            tmp2 = check_exp(k->child->next);
        if (strcmp(k->child->next->next->name, "Exp") == 0)
            tmp3 = check_exp(k->child->next->next);
        if (strcmp(k->child->next->name, "ASSIGNOP") == 0)
        {
            if (tmp1 && tmp3)
            {
                if (!check_error5(tmp1, tmp3, k->child->line_num))
                    return NULL;
                if (!check_error6(k->child, k->child->line_num))
                    return NULL;
            }
        }
        else if (strcmp(k->child->name, "Exp") == 0 && tmp1 && tmp3)
        {
            if (!check_error7(tmp1, tmp3, k->child->line_num))
                return NULL;
        }
    }
    else
    { // ID LP Args RP | Exp LB Exp RB
        if (strcmp(k->child->name, "Exp") == 0)
            check_exp(k->child);
        if (strcmp(k->child->next->next->name, "Exp") == 0)
            check_exp(k->child->next->next);
        if (strcmp(k->child->next->next->name, "Args") == 0)
        { // Args = Exp COMMA Args | Exp
            if (!check_error2(k->child->val.char_val, k->child->line_num))
                return NULL;
            ListNode *para = search_listnode(symbol_define_table,
                                             k->child->val.char_val, FUNC)
                                 ->type->func.para;
            for (Node *i = k->child->next->next;; i = i->child->next->next)
            {
                Type *now = check_exp(i->child);
                //         printf("%d == %d\n",para->type->kind,now->kind);
                if (!check_error9(para, now, k->child->line_num))
                    return NULL;
                para = para->next;
                if (i->child_num == 1)
                    break;
            }
            if (!check_error9(para, now, k->child->line_num))
                return NULL;
        }
        if (strcmp(k->child->next->name, "LB") == 0)
        {
            Type *now = check_exp(k->child);
            if (!check_error10(now, k->child->line_num))
                return NULL;
        }
    }
    return now;
}

bool type_equal(Type *a, Type *b)
{
    if (a == NULL && b != NULL)
        return false;
    if (a != NULL && b == NULL)
        return false;
    if (a->kind == BASIC && b->kind == BASIC)
    {
        if (a->basic == b->basic)
            return true;
    }
    if (a->kind == ARRAY && b->kind == ARRAY)
    {
        Type *tmpa = a->array.elem;
        Type *tmpb = b->array.elem;
        while (tmpa && tmpb)
        {
            tmpa = tmpa->array.elem;
            tmpb = tmpb->array.elem;
        }
        if (tmpa == NULL && tmpb == NULL)
            return true; //数组维数相同
    }
    if (a->kind == STRUCTURE && b->kind == STRUCTURE)
    {
        ListNode *j = b->structure;
        for (ListNode *i = a->structure; i; i = i->next)
        {
            if (j == NULL)
                return false;
            if (!type_equal(i->type, j->type))
                return false;
            j = j->next;
        }
        if (j != NULL)
            return false;
        return true;
    } //结构体类型等价
    if (a->kind == FUNC && b->kind == FUNC)
    {
        return type_equal(a->func.func_type, b->func.func_type);
    }
    return false;
}

bool if_left(Node *k)
{
    // Exp DOT ID | ID
    if (strcmp(k->child->name, "ID") == 0 && k->child_num == 1)
        return true;
    if (k->child_num == 3 && (k->child->next->name, "DOT") == 0)
        return if_left(k->child);
    return false;
}

bool check_error1(char *name, int line_num)
{
    ListNode *tmp1 = search_listnode(symbol_define_table, name, BASIC);
    ListNode *tmp2 = search_listnode(symbol_define_table, name, ARRAY);
    ListNode *tmp3 = search_listnode(symbol_define_table, name, STRUCTURE);
    if (tmp1 == NULL && tmp2 == NULL && tmp3 == NULL)
    {
        error(1, line_num, "Undefined variable", name);
        return false;
    }
    return true;
}

bool check_error2(char *name, int line_num)
{
    ListNode *tmp = search_listnode(symbol_define_table, name, FUNC);
    if (tmp == NULL)
    {
        error(2, line_num, "Undefined function", name);
        return false;
    }
    return true;
}

bool check_error3(char *name, int line_num)
{
    ListNode *tmp1 = search_listnode(symbol_define_table, name, BASIC);
    ListNode *tmp2 = search_listnode(symbol_define_table, name, ARRAY);
    ListNode *tmp3 = search_listnode(symbol_define_table, name, STRUCTURE);
    if (tmp1 != NULL || tmp2 != NULL || tmp3 != NULL)
    {
        error(3, line_num, "Redefined variable", name);
        return false;
    }
    return true;
}

bool check_error4(char *name, int line_num)
{
    ListNode *tmp = search_listnode(symbol_define_table, name, FUNC);
    if (tmp != NULL)
    {
        error(4, line_num, "Rndefined function", name);
        return false;
    }
    return true;
}

bool check_error5(Type *a, Type *b, int line_num)
{
    if (!type_equal(a, b))
    {
        error(5, line_num, "Type mismatched for assignment", "");
        return false;
    }
    return true;
}

bool check_error6(Node *k, int line_num)
{
    if (!if_left(k))
    {
        error(6, line_num, "The left-hand side of an assignment \
must be a variable.",
              "");
        return false;
    }
    return true;
}

bool check_error7(Type *a, Type *b, int line_num)
{
    if (!type_equal(a, b))
    {
        error(7, line_num, "Type mismatched for operands", "");
        return false;
    }
    return true;
}

bool check_error8(Type *a, Type *b, int line_num)
{
    if (!type_equal(a, b))
    {
        error(8, line_num, "Type mismatched for return.", "");
        return false;
    }
    return true;
}

bool check_error9(ListNode *a, Type *b, int line_num)
{
    if (a == NULL)
    {
        error(9, line_num, "Function is not applicable for arguments", "");
        return false;
    }
    if (!type_equal(a->type, b))
    {
        error(9, line_num, "Function is not applicable for arguments", "");
        return false;
    }
    return true;
}

bool check_error10(Type *now, int line_num)
{
    if (now->kind != ARRAY)
    {
        error(10, line_num, "variable is not an array", "");
        return false;
    }
    return true;
}

void error(int k, int line, char *s, char *y)
{
    printf("Error type %d at Line %d: %s %s\n", k, line, s, y);
}