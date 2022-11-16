#include "semantic.h"
#include "node.h"

//#define DEBUG

int semantic_right = 1;

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
        p->func.func_type = copy_type(x->func.func_type);
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
    p->para_no = x->para_no;
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
    p->para_no = 0; //表明不是函数参数
}

void deal_ExtDef(Node *k)
{
    // ExtDef = Specifier ExtDecList SEMI | Specifier SEMI |
    // Specifier FunDec CompSt | Specifier FunDec SEMI
    if (strcmp(k->child->next->name, "FunDec") == 0)
    {
        Type *type = deal_Specifier(k->child, false);
        if (type == NULL)
            return;
        //注意，已添加，只能改变内部值
        if (strcmp(k->child->next->next->name, "SEMI") == 0)
        { //声明
            ListNode *fun = deal_FunDec(k->child->next, type, false);
            fun->type->func.func_line = k->child->next->line_num;
            if (fun)
            {
                stack_pop();
                if (!check_error19(fun, k->child->next->line_num))
                    return;
                insert_listnode(fun, &announce_stack[announce_top]); //添加自己
            }
        }
        else
        {
            // CompSt = LC DefList StmtList RC 函数具体定义，先声明后使用
            ListNode *fun = deal_FunDec(k->child->next, type, true);
            if (fun)
            {
                Node *compst = k->child->next->next;
                deal_CompSt(compst, type);
                //解析内部定义
                compst->symbol = define_stack[define_top]; //将符号表附加到compst
                stack_pop();
            }
        }
    }
    else if (k->child_num == 3)
    {
        Type *type = deal_Specifier(k->child, true);
        if (type == NULL)
            return;
        deal_ExtDecList(k->child->next, type);
    }
    else
        deal_Specifier(k->child, true);
    return;
}

ListNode *deal_FunDec(Node *k, Type *type, bool flag)
{
// FunDec = ID LP VarList RP| ID LP RP //函数及参数列表
#ifdef DEBUG
    printf("FUNC %s\n", k->child->val.char_val);
#endif
    if (flag)
    {
        if (!check_error4(k->child->val.char_val, k->child->line_num))
            return NULL;
    }
    Type *funcself = new_type(FUNC); //函数类型
    ListNode *fun = new_listnode(k->child->val.char_val, funcself);
    fun->type->func.func_type = type; //返回值类型
    if (flag)
        insert_listnode(fun, &define_stack[define_top]); //添加自己
    stack_push();
    if (k->child_num == 4)
    { //有参数
        ListNode *now = deal_VarList(k->child->next->next);
        fun->type->func.para = now; //参数列表
    }
    return fun;
}

Type *deal_Specifier(Node *k, bool flag)
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
        now = deal_StructSpecifier(k->child, flag);
    }
    return now;
}

Type *deal_StructSpecifier(Node *k, bool flag)
{
    // StructSpecifier = STRUCT OptTag LC DefList RC | STRUCT Tag
    Type *list = new_type(STRUCTURE); //结构体成员列表
    if (k->child_num == 2)
    { // STRUCT Tag
        if (!check_error17(k->child->next->child->val.char_val,
                           k->child->next->child->line_num))
            return NULL;
        ListNode *tmp = search_listnode(define,
                                        k->child->next->child->val.char_val,
                                        STRUCTURE, false);
        if (tmp != NULL)
            list->structure = tmp->type->structure;
        //如果之前定义了就找到定义
        else
        {
            ListNode *p = new_listnode(k->child->next->child
                                           ->val.char_val,
                                       list);
            insert_listnode(p, &announce_stack[announce_top]);
#ifdef DEBUG
            printf("STRUCT %s\n", p->name);
#endif
        }
    }
    else
    { // STRUCT OptTag LC DefList RC
        // OptTag = ID | empty
        Node *opttag = k->child->next;
        if (opttag->child_num != 1 || opttag->child)
        { //有名字
            if (flag)
            {
                if (!check_error16(k->child->next->child->val.char_val,
                                   k->child->next->child->line_num))
                    return NULL;
            }
            stack_push();
            ListNode *deflist = deal_DefList(k->child->next->next->next, true);
            stack_pop();
            list->structure = deflist;
            ListNode *p = new_listnode(k->child->next->child->val.char_val,
                                       list);
            insert_listnode(p, &define_stack[define_top]);
        }
        else
        {
            stack_push();
            ListNode *deflist = deal_DefList(k->child->next->next->next, true);
            list->structure = deflist;
            stack_pop();
        }
    }
    return list;
}

ListNode *deal_DefList(Node *k, bool flag)
{ //函数与结构体内部定义，返回参数列表
    // DefList = Def DefList | empty
    // Def = Specifier DecList SEMI//变量声明
    if (k->child_num == 1 && k->child == NULL)
        return NULL;
    ListNode *head = NULL;
    ListNode *last = NULL;
    for (Node *i = k; i; i = i->child->next)
    {
        if (i->child_num == 1 && i->child == NULL)
            break;
        // DecList = Dec | Dec COMMA DecList
        Type *spe = deal_Specifier(i->child->child, true);
        //注意：函数参数类型可能是结构体，此时要找到之前的定义
        if (spe == NULL)
            continue;
        for (Node *j = i->child->child->next->child;;
             j = j->next->next->child) //每个变量的名称
        {
            ListNode *now = deal_Dec(j, spe, flag);
            if (now != NULL)
            {
                insert_listnode(now, &define_stack[define_top]);
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
    if (k->child_num == 1 && k->child == NULL)
        return;
    for (Node *i = k;; i = i->child->next)
    {
        if (i->child_num == 1 && i->child == NULL)
            break;
        Node *stmt = i->child;
        deal_stmt(stmt, t);
    }
}

ListNode *deal_CompSt(Node *compst, Type *t)
{
    // CompSt = LC DefList StmtList RC 函数具体定义，先声明后使用
    deal_DefList(compst->child->next, false);
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
        Type *spe = deal_Specifier(i->child, true);
        ListNode *var = deal_VarDec(i->child->next, spe, false);
        if (var != NULL)
        {
            insert_listnode(var, &define_stack[define_top]);
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
    // ExtDecList = VarDec | VarDec COMMA ExtDecList
    if (k->child_num == 1 && k->child == NULL)
        return;
    else
    {
        for (Node *i = k;; i = i->child->next->next)
        {
            //以逗号隔开的每个简单参数的头
            ListNode *now = deal_VarDec(i->child, type, false);
            if (now != NULL)
                insert_listnode(now, &define_stack[define_top]);
            if (i->child->next == NULL)
                break;
        }
    }
}

ListNode *deal_Dec(Node *k, Type *type, bool flag)
{
    // Dec = VarDec | VarDec ASSIGNOP Exp
    ListNode *tmp1 = deal_VarDec(k->child, type, flag);
    if (tmp1 == NULL)
        return NULL;
    if (k->child->next)
    {
        if (flag)
            check_error15("", k->child->next->line_num);
        ListNode *tmp2 = deal_exp(k->child->next->next);
        if (tmp1 && tmp2)
        {
            if (!check_error5(tmp1->type, tmp2->type, k->child->line_num))
                return NULL;
            if (!check_error6(k->child, k->child->line_num))
                return NULL;
        }
    }
    //   insert_listnode(p, &define_stack[define_top]);
    return copy_listnode(tmp1);
}

ListNode *deal_VarDec(Node *k, Type *type, bool flag)
{
    // VarDec = ID | VarDec LB INT RB
    if (strcmp(k->child->name, "ID") == 0)
    { // ID头
#ifdef DEBUG
        printf("ID : %s\n", k->child->val.char_val);
#endif
        if (!flag)
        { //函数中的定义
            if (!check_error3(k->child->val.char_val, k->child->line_num))
                return NULL;
        }
        else
        { //结构体中的定义
            if (!check_error15(k->child->val.char_val, k->child->line_num))
                return NULL;
        }
        ListNode *now = new_listnode(k->child->val.char_val, type);
        return now; //数组
    }
    else
    {                                                    //数组后半部分
        ListNode *p = deal_VarDec(k->child, type, flag); //头节点，也就是TYPE
        if (p == NULL)
            return NULL;
        Type *tmp = p->type;
        Type *pre = NULL;
        while (tmp->kind == ARRAY)
        {
            pre = tmp;
            tmp = tmp->array.elem;
        }                //最后一个数组维度
        Type *now = tmp; //继承最开头的类型
        tmp = new_type(ARRAY);
        tmp->array.size = k->child->next->next->val.int_val;
        tmp->array.elem = now;
        //         printf("[%d]\n", now->array.size);
        if (pre == NULL)
            p->type = tmp;
        else
            pre->array.elem = tmp;
        return p;
    }
}

void deal_stmt(Node *k, Type *t)
{
    // Stmt = Exp SEMI| CompSt| RETURN Exp SEMI | IF LP Exp RP Stmt
    //|IF LP Exp RP Stmt ELSE Stmt | WHILE LP Exp RP Stmt //使用
    if (k->child_num == 7)
    { // IF LP Exp RP Stmt ELSE Stmt
        deal_exp(k->child->next->next);
        deal_stmt(k->child->next->next->next->next, t);
        deal_stmt(k->child->next->next->next->next->next->next, t);
    }
    else if (k->child_num == 5)
    {
        // IF LP Exp RP Stmt | WHILE LP Exp RP Stmt
        deal_exp(k->child->next->next);
        deal_stmt(k->child->next->next->next->next, t);
    }
    else if (k->child_num == 3)
    {
        // RETURN Exp SEMI
        ListNode *now = deal_exp(k->child->next);
        if (now)
        {
            if (!check_error8(now->type, t, k->child->line_num))
                return;
        }
        else
            check_error8(NULL, t, k->child->line_num);
    }
    else if (k->child_num == 2)
        // Exp SEMI
        deal_exp(k->child);
    else
    {
        stack_push();
        deal_CompSt(k->child, t);
        k->child->symbol = define_stack[define_top];
        //将符号表附加到compst
        stack_pop();
    }
}

ListNode *deal_exp(Node *k)
{ //返回的是运算结果类型
    // Exp = Exp ASSIGNOP Exp| Exp AND Exp | Exp OR Exp
    //| Exp RELOP Exp | Exp PLUS Exp | Exp MINUS Exp
    //| Exp STAR Exp | Exp DIV Exp | LP Exp RP | MINUS Exp
    //| NOT Exp | ID LP Args RP | ID LP RP | Exp LB Exp RB
    //| Exp DOT ID | ID | INT | FLOAT
    ListNode *now = (ListNode *)malloc(sizeof(ListNode));
    if (k->child_num == 1)
    { // ID | INT | FLOAT
        if (strcmp(k->child->name, "INT") == 0)
        {
            now->type = new_type(BASIC);
            now->type->basic = Basic_int;
        }
        else if (strcmp(k->child->name, "FLOAT") == 0)
        {
            now->type = new_type(BASIC);
            now->type->basic = Basic_float;
        }
        else if (strcmp(k->child->name, "ID") == 0)
        {
            if (!check_error1(k->child->val.char_val, k->child->line_num))
                return NULL;
            now = search_all_listnode(define, k->child->val.char_val, false);
        }
    }
    else if (k->child_num == 2) // NOT Exp | MINUS Exp
        now = deal_exp(k->child->next);
    else if (k->child_num == 3)
    { // Exp ASSIGNOP Exp | Exp AND Exp | Exp OR Exp
      //| Exp RELOP Exp | Exp PLUS Exp | Exp MINUS Exp
      //| Exp STAR Exp | Exp DIV Exp | LP Exp RP
      //| ID LP RP | Exp DOT ID
        ListNode *tmp1 = NULL, *tmp2 = NULL, *tmp3 = NULL;
        //    printf("\n[%s %s %s]\n",k->child->name,k->child->next->name,k->child->next->next->name);
   //     for (int i = define_top; i >= 0; i--)
    //        print_avl_tree(define_stack[i]);
        if (strcmp(k->child->name, "Exp") == 0)
            tmp1 = deal_exp(k->child);
        if (strcmp(k->child->next->name, "Exp") == 0)
            tmp2 = deal_exp(k->child->next);
        if (strcmp(k->child->next->next->name, "Exp") == 0)
            tmp3 = deal_exp(k->child->next->next);
        if (strcmp(k->child->next->name, "ASSIGNOP") == 0)
        { // Exp ASSIGNOP Exp
            if (tmp1 && tmp3)
            {
                if (!check_error5(tmp1->type, tmp3->type, k->child->line_num))
                    return NULL;
                if (!check_error6(k->child, k->child->line_num))
                    return NULL;
            } //////////////
            else
                return NULL;
        }
        else if (strcmp(k->child->next->name, "RELOP") == 0)
        { // Exp RELOP Exp
            if (tmp1 && tmp3)
            {
                if (!check_error7(tmp1->type, tmp3->type, k->child->line_num))
                    return NULL;
                now->type = new_type(BASIC);
                now->type->basic = Basic_int;
            }
            else
                check_error7(NULL, new_type(BASIC), k->child->line_num);
        }
        else if (strcmp(k->child->name, "ID") == 0)
        { // ID LP RP
            if (!check_error11(k->child->val.char_val, k->child->line_num))
                return NULL;
            if (!check_error2(k->child->val.char_val, k->child->line_num))
                return NULL;
            now->type = search_type(define, k->child->val.char_val,
                                    false)
                            ->func.func_type;
        }
        else if (strcmp(k->child->name, "LP") == 0)
        { // LP Exp RP
            now = tmp2;
        }
        else if (strcmp(k->child->next->name, "DOT") == 0)
        { // Exp DOT ID
            now = deal_exp(k->child);
            if (!check_error13(now, k->child->line_num))
                return NULL;
            now = now->type->structure;
            if (!check_error14(&now, k->child->next->next->val.char_val,
                               k->child->line_num))
                return NULL;
        }
        else if (tmp1 && tmp3)
        {
            if (!check_error7(tmp1->type, tmp3->type, k->child->line_num))
                return NULL;
            now = tmp1;
        }
    }
    else
    { // ID LP Args RP | Exp LB Exp RB
        if (strcmp(k->child->next->next->name, "Args") == 0)
        { // Args = Exp COMMA Args | Exp
            if (!check_error11(k->child->val.char_val, k->child->line_num))
                return NULL;
            if (!check_error2(k->child->val.char_val, k->child->line_num))
                return NULL;
            now = search_listnode(define, k->child->val.char_val, FUNC, false);
            //             printf("NOW = :\n");
            //             print_avl_listnode(now);
            ListNode *para = now->type->func.para;
            Node *i = k->child->next->next;
            for (;; i = i->child->next->next, para = para->next)
            {
                ListNode *tmp = deal_exp(i->child);

                if (!check_error9(para, tmp->type, k->child->line_num))
                    return NULL;

                if (i->child_num == 1)
                {

                    if (!check_error9(para->next, NULL, k->child->line_num))
                        return NULL;
                    break;
                }
                else if (para == NULL)
                {
                    if (!check_error9(NULL, new_type(BASIC), k->child->line_num))
                        return NULL;
                    break;
                }
            }
            now = new_listnode("", now->type->func.func_type);
        }
        else if (strcmp(k->child->next->name, "LB") == 0)
        { // Exp LB Exp RB
            ListNode *tmp1 = deal_exp(k->child);
            if (tmp1 == NULL)
                return NULL;
            if (!check_error10(tmp1->type, k->child->line_num))
                return NULL;
            ListNode *tmp2 = deal_exp(k->child->next->next);
            if (!check_error12(tmp2->type, k->child->next->next->line_num))
                return NULL;
            now->type = tmp1->type->array.elem;
        }
    }
    return now;
}

void read(Node *k)
{
    if (k == NULL)
        return;
    //    printf("now read %s\n", k->name);
    if (strcmp(k->name, "ExtDef") == 0)
        deal_ExtDef(k);
    else
        for (Node *i = k->child; i; i = i->next)
        {
            read(i);
        }
    //  if (strcmp(k->name, "Program") == 0)
    //      k->symbol = define_stack[define_top]; //所有全局变量
}

void insert_listnode(ListNode *k, AVL_node **table)
{
#ifdef DEBUG
    printf("try insert %s\n", k->name);
#endif
    Type *now = search_type(define, k->name, true);
    if (now)
    {
#ifdef DEBUG
        printf("%s already exist\n", k->name);
#endif
        return;
    }
    else
    {
#ifdef DEBUG
        printf("%s doesn't exist\n", k->name);
#endif
        Insert(table, k);
    }
    return;
}

bool type_equal(Type *a, Type *b)
{
    if (a == NULL && b != NULL)
        return false;
    if (a != NULL && b == NULL)
        return false;
    if (a == NULL && b == NULL)
        return true;
    if (a->kind == BASIC && b->kind == BASIC)
    {
        if (a->basic == b->basic)
            return true;
    }
    if (a->kind == ARRAY && b->kind == ARRAY)
    {
        Type *tmpa = a->array.elem;
        Type *tmpb = b->array.elem;
        return type_equal(tmpa, tmpb);
    }
    if (a->kind == STRUCTURE && b->kind == STRUCTURE)
    {
        ListNode *j = b->structure;
        ListNode *i = a->structure;
        for (; i && j; i = i->next, j = j->next)
        {

            if (!type_equal(i->type, j->type))
                return false;
        }
        if (i != NULL || j != NULL)
            return false;
        return true;
    } //结构体类型等价
    if (a->kind == FUNC && b->kind == FUNC)
    {
        if (!type_equal(a->func.func_type, b->func.func_type))
            return false; //返回类型要一致
        ListNode *i = a->func.para;
        ListNode *j = b->func.para;
        for (; i && j; i = i->next, j = j->next)
        {
            if (!type_equal(i->type, j->type))
                return false;
        }
        if (i != NULL || j != NULL)
            return false;
        return true;
    }
    return false;
}

bool if_left(Node *k)
{
    // Exp DOT ID | ID | Exp LB Exp RB
    if (strcmp(k->child->name, "ID") == 0 && k->child_num == 1)
        return true;
    if (k->child_num == 3 && strcmp(k->child->next->name, "DOT") == 0)
        return if_left(k->child);
    if (k->child_num == 4 && strcmp(k->child->next->name, "LB") == 0)
        return if_left(k->child);
    return false;
}

bool check_error1(char *name, int line_num)
{
    ListNode *tmp1 = search_listnode(define, name, BASIC, false);
    ListNode *tmp2 = search_listnode(define, name, ARRAY, false);
    ListNode *tmp3 = search_listnode(define, name, STRUCTURE, false);
    //在已定义的符号表内寻找三个类型的符号，最后一个参数为2.2要求
    if (tmp1 == NULL && tmp2 == NULL && tmp3 == NULL)
    {
        error(1, line_num, "Undefined variable", name);
        return false;
        //如果没找到说明符号未定义
    }
    return true;
}

bool check_error2(char *name, int line_num)
{
    ListNode *tmp = search_listnode(define, name, FUNC, false);
    if (tmp == NULL)
    {
        error(2, line_num, "Undefined function", name);
        return false;
    }
    return true;
}

bool check_error3(char *name, int line_num)
{
    ListNode *tmp1 = search_listnode(define, name, BASIC, true);
    //最后一个参数为true，BASIC基本类型作用域可以嵌套，所以只在本层寻找是否有重定义
    ListNode *tmp2 = search_listnode(define, name, ARRAY, true);
    ListNode *tmp3 = search_listnode(define, name, STRUCTURE, false);
    //最后一个参数为false，结构体定义是全局的，所以要从里到外查看是否有重定义
    if (tmp1 != NULL || tmp2 != NULL || tmp3 != NULL)
    {
        error(3, line_num, "Redefined variable", name);
        return false;
    }
    return true;
}

bool check_error4(char *name, int line_num)
{
    ListNode *tmp = search_listnode(define, name, FUNC, false);
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
        if (b != NULL)
        {
            error(9, line_num, "Function is not applicable for arguments", "");
            return false;
        }
        else
            return true;
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
        error(10, line_num, "variable", "is not an array");
        return false;
    }
    return true;
}

bool check_error11(char *name, int line_num)
{
    ListNode *tmp = search_listnode(define, name, FUNC, false);
    ListNode *tmp1 = search_listnode(define, name, BASIC, false);
    ListNode *tmp2 = search_listnode(define, name, ARRAY, false);
    ListNode *tmp3 = search_listnode(define, name, STRUCTURE, false);
    if ((tmp1 != NULL || tmp2 != NULL || tmp3 != NULL) && (tmp == NULL))
    {
        error(11, line_num, name, "is not a function");
        if (tmp1 != NULL)
            printf("name is a basic\n"), print_avl_listnode(tmp1);
        if (tmp2 != NULL)
            printf("name is a array\n"), print_avl_listnode(tmp2);
        if (tmp3 != NULL)
            printf("name is a structure\n"), print_avl_listnode(tmp3);
        return false;
    }
    return true;
}

bool check_error12(Type *now, int line_num)
{
    if (now->basic != Basic_int)
    {
        error(12, line_num, "parameter", "is not an integer.");
        return false;
    }
    return true;
}

bool check_error13(ListNode *a, int line_num)
{
    if (a->type->kind != STRUCTURE)
    {
        error(13, line_num, "Illegal use of '.'", "");
        return false;
    }
    return true;
}

bool check_error14(ListNode **a, char *name, int line_num)
{
    for (; (*a); (*a) = (*a)->next)
    {
        if (strcmp((*a)->name, name) == 0)
            return true;
    }
    error(14, line_num, "Non-existent field", name);
    return false;
}

bool check_error15(char *name, int line_num)
{
    if (strcmp(name, "") != 0)
    {
        ListNode *tmp1 = search_listnode(define, name, BASIC, true);
        ListNode *tmp2 = search_listnode(define, name, ARRAY, true);
        ListNode *tmp3 = search_listnode(define, name, STRUCTURE, true);
        if (tmp1 != NULL || tmp2 != NULL || tmp3 != NULL)
        {
            error(15, line_num, "Redefined field", name);
            return false;
        }
        return true;
    }
    else
    {
        error(15, line_num, "初始化定义", name);
        return false;
    }
}

bool check_error16(char *name, int line_num)
{
    ListNode *tmp1 = search_listnode(define, name, BASIC, false);
    ListNode *tmp2 = search_listnode(define, name, ARRAY, false);
    ListNode *tmp3 = search_listnode(define, name, STRUCTURE, false);
    if (tmp1 != NULL || tmp2 != NULL || tmp3 != NULL)
    {
        error(16, line_num, "Duplicated name", name);
        return false;
    }
    return true;
}

bool check_error17(char *name, int line_num)
{
    ListNode *tmp3 = search_listnode(define, name, STRUCTURE, false);
    if (tmp3 == NULL)
    {
        error(17, line_num, "Undefined structure", name);
        return false;
    }
    return true;
}

bool check_error18()
{
    check_announce(announce_stack[announce_top], FUNC);
}

bool check_error19(ListNode *now, int line_num)
{
    ListNode *tmp1 = search_listnode(define, now->name, FUNC, false);
    ListNode *tmp2 = search_listnode(announce, now->name, FUNC, false);
    ListNode *tmp = NULL;
    if (tmp1)
        tmp = tmp1;
    if (tmp2)
        tmp = tmp2;
    if (!tmp)
        return true; //没有定义或声明
    if (!type_equal(tmp->type, now->type))
    {
        error(19, line_num, "Inconsistent declaration of function",
              now->name);
        return false;
    }
    return true;
}

void error(int k, int line, char *s, char *y)
{
    printf("Error type %d at Line %d: %s %s\n", k, line, s, y);
    semantic_right = 0;
}