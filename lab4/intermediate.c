#include "intermediate.h"
#include "semantic.h"

//#define DEBUG
#define SIZE 4

InterCode *intercode_head = NULL;
InterCode *intercode_tail = NULL;
int label_count = 1; //标号数目
int tmp_count = 1;   //临时变量数目
int para_count = 1;  //函数参数数目

Operand *zero;
Operand *one;
Operand *size;

void trans_init()
{
    zero = new_constant(0);
    one = new_constant(1);
    size = new_constant(SIZE);
}

void insert_intercode(InterCode *k)
{
    if (intercode_head == NULL)
        intercode_head = intercode_tail = k;
    else
    {
        intercode_tail->next = k;
        k->pre = intercode_tail;
        intercode_tail = k;
    }
    //  print_intercode(k);
}

void remove_intercode(InterCode *k)
{
    if (k == intercode_head)
        intercode_head = k->next;
    else if (k == intercode_tail)
        k->pre->next = NULL;
    else
    {
        k->pre->next = k->next;
        k->next->pre = k->pre;
    }
    //虽然从链表里去掉了k，但k仍在，可以访问它原来的pre和next
}

Operand *new_operand(enum Operand_kind kind, int type, char *name)
{
    Operand *p = (Operand *)malloc(sizeof(Operand));
    strcpy(p->name, "");
    p->kind = kind;
    p->type = type;
    p->variable = NULL;
    p->next = NULL;
    p->offset = -1;
    if (kind == CONSTANT_operand)
    {
    }
    else if (kind == TMP_operand)
    {
        p->value = tmp_count++;
    }
    else if (kind == PARA_operand)
    {
    }
    else if (kind == FUNCTION_operand)
    {
        strcpy(p->name, name);
    }
    else if (kind == LABEL_operand)
    {
        p->value = label_count++;
    }

    return p;
}
void new_intercode(enum Intercode_kind kind, Operand *res,
                   Operand *op1, Operand *op2, Operand *op3)
{
    InterCode *p = (InterCode *)malloc(sizeof(InterCode));
    p->kind = kind;
    p->pre = p->next = NULL;
    if (kind == LABEL_in || kind == FUNCTION_in || kind == GOTO_in || kind == RETURN_in || kind == ARG_in || kind == PARAM_in || kind == READ_in || kind == WRITE_in)
    {
        p->singop.op = op1;
        p->para_num = 1;
    }
    else if (kind == ASSIGN_in || kind == DEC_in || kind == CALL_in)
    {
        p->binop.op1 = op1;
        p->binop.op2 = op2;
        p->para_num = 2;
    }
    else if (kind == PLUS_in || kind == MINUS_in || kind == STAR_in || kind == DIV_in)
    {
        p->triop.op1 = op1;
        p->triop.op2 = op2;
        p->triop.result = res;
        p->para_num = 3;
    }
    else if (kind == IF_in)
    {
        p->recop.op1 = op1;
        p->recop.op2 = op2;
        p->recop.op3 = op3;
        p->recop.relop = res;
        p->para_num = 4;
    }
    insert_intercode(p);
}

Operand *new_label()
{
    return new_operand(LABEL_operand, Normal, "");
}

Operand *new_tmp()
{
    return new_operand(TMP_operand, Normal, "");
}

Operand *new_para()
{
    return new_operand(PARA_operand, Normal, "");
}

Operand *new_constant(int k)
{
    Operand *now = new_operand(CONSTANT_operand, Normal, "");
    now->value = k;
    return now;
}

Operand *new_function(char *name)
{
    return new_operand(FUNCTION_operand, Normal, name);
}

void print_operand(Operand *k)
{
    if (k->type == Address)
        printf("&");
    else if (k->type == Star)
        printf("*");

    int kind = k->kind;
    if (kind == FUNCTION_operand)
        printf("%s", k->name);
    else if (kind == CONSTANT_operand)
        printf("#%d", k->value);
    else if (kind == TMP_operand)
        printf("t%d", k->value);
    else if (kind == PARA_operand)
        printf("v%d", abs(k->value));
    else if (kind == LABEL_operand)
        printf("label%d", k->value);
}

void print_intercode(InterCode *k)
{
    int kind = k->kind;
    // singop
    if (kind == LABEL_in)
    {
        printf("LABEL ");
        print_operand(k->singop.op);
        printf(" :");
    }
    else if (kind == FUNCTION_in)
    {
        printf("FUNCTION ");
        print_operand(k->singop.op);
        printf(" :");
    }
    else if (kind == GOTO_in)
    {
        printf("GOTO ");
        print_operand(k->singop.op);
    }
    else if (kind == RETURN_in)
    {
        printf("RETURN ");
        print_operand(k->singop.op);
    }
    else if (kind == ARG_in)
    {
        printf("ARG ");
        print_operand(k->singop.op);
    }
    else if (kind == PARAM_in)
    {
        printf("PARAM ");
        print_operand(k->singop.op);
    }
    else if (kind == READ_in)
    {
        printf("READ ");
        print_operand(k->singop.op);
    }
    else if (kind == WRITE_in)
    {
        printf("WRITE ");
        print_operand(k->singop.op);
    }
    // binop
    else if (kind == ASSIGN_in)
    {
        print_operand(k->binop.op1);
        printf(" := ");
        print_operand(k->binop.op2);
    }
    else if (kind == DEC_in)
    {
        printf("DEC ");
        print_operand(k->binop.op1);
        printf(" %d", k->binop.op2->value);
        //只能单独打印，因为DEC中的常数前不加#
    }
    else if (kind == CALL_in) //单独考虑read和write
    {
        if (strcmp(k->binop.op2->name, "read") == 0)
        {
            printf("READ ");
            print_operand(k->binop.op1);
        }
        else if (strcmp(k->binop.op2->name, "write") == 0)
        {
            printf("WRITE ");
            print_operand(k->binop.op1);
        }
        else
        {
            print_operand(k->binop.op1);
            printf(" := CALL ");
            print_operand(k->binop.op2);
        }
    }
    // triop
    else if (kind == PLUS_in)
    {
        print_operand(k->triop.result);
        printf(" := ");
        print_operand(k->triop.op1);
        printf(" + ");
        print_operand(k->triop.op2);
    }
    else if (kind == MINUS_in)
    {
        print_operand(k->triop.result);
        printf(" := ");
        print_operand(k->triop.op1);
        printf(" - ");
        print_operand(k->triop.op2);
    }
    else if (kind == STAR_in)
    {
        print_operand(k->triop.result);
        printf(" := ");
        print_operand(k->triop.op1);
        printf(" * ");
        print_operand(k->triop.op2);
    }
    else if (kind == DIV_in)
    {
        print_operand(k->triop.result);
        printf(" := ");
        print_operand(k->triop.op1);
        printf(" / ");
        print_operand(k->triop.op2);
    }
    // recop
    else if (kind == IF_in)
    {
        printf("IF ");
        print_operand(k->recop.op1);
        printf(" ");
        print_operand(k->recop.relop);
        printf(" ");
        print_operand(k->recop.op2);
        printf(" GOTO ");
        print_operand(k->recop.op3);
    }
    printf("\n");
}

void print_all_intercode()
{
    InterCode *p = intercode_head;
    while (p != NULL)
    {
        print_intercode(p);
        p = p->next;
    }
}

int size_of(Type *k)
{
    if (k->kind == BASIC)
        return SIZE;
    else if (k->kind == ARRAY)
        return k->array.size * size_of(k->array.elem);
    else if (k->kind == STRUCTURE)
    {
        int size = 0;
        for (ListNode *i = k->structure; i; i = i->next)
            size += size_of(i->type);
        return size;
    }
    else if (k->kind == FUNC)
        return 0;
}

void trans_ExtDef(Node *k)
{
    // ExtDef = Specifier ExtDecList SEMI | Specifier SEMI |
    // Specifier FunDec CompSt | Specifier FunDec SEMI
    if (strcmp(k->child->next->name, "FunDec") == 0)
    {
        stack_push();
        stack_change(k->child->next->next->symbol); //将符号表填回
        trans_FunDec(k->child->next);
        if (strcmp(k->child->next->next->name, "CompSt") == 0)
            // CompSt = LC DefList StmtList RC 函数具体定义，先声明后使用
            trans_CompSt(k->child->next->next);
        stack_pop();
    }
    else if (k->child_num == 3)
    { // Specifier ExtDecList SEMI
        trans_ExtDecList(k->child->next);
    }
    return;
}

void trans_FunDec(Node *k)
{
    // FunDec = ID LP VarList RP| ID LP RP //函数及参数列表
    Operand *func = new_function(k->child->val.char_val);
    new_intercode(FUNCTION_in, NULL, func, NULL, NULL);
    if (k->child_num == 4)
    { //有参数
        func->value = trans_VarList(k->child->next->next);
    }
    return;
}

int trans_VarList(Node *k)
{
    int num = 0;
    // ParamDec COMMA VarList| ParamDec
    for (Node *i = k->child;; i = i->next->next->child)
    {
        //每一个参数定义
        // Specifier VarDec
        num++; //记录参数个数
        ListNode *p = trans_VarDec(i->child->next);
        p->para_no = -para_count++; //记录这是第几个参数
        //负数代表是函数参数，自己就是地址
        Operand *param = new_para();
        param->value = p->para_no;
        new_intercode(PARAM_in, NULL, param, NULL, NULL);
        if (i->next == NULL)
            break;
    }
    return num;
}

void trans_DefList(Node *k)
{ //函数与结构体内部定义，返回参数列表
    // DefList = Def DefList | empty
    // Def = Specifier DecList SEMI//变量声明
    if (k->child_num == 1 && k->child == NULL)
        return;
    for (Node *i = k; i; i = i->child->next)
    {
        if (i->child_num == 1 && i->child == NULL)
            break;
        // DecList = Dec | Dec COMMA DecList
        for (Node *j = i->child->child->next->child;;
             j = j->next->next->child) //每个变量的名称
        {
            trans_Dec(j);
            if (j->next == NULL)
                break;
        }
    }
    return;
}

void trans_StmtList(Node *k)
{ //函数内部使用
    // StmtList = Stmt StmtList | empty
    if (k->child_num == 1 && k->child == NULL)
        return;
    for (Node *i = k;; i = i->child->next)
    {
        if (i->child_num == 1 && i->child == NULL)
            break;
        Node *stmt = i->child;
        trans_stmt(stmt);
    }
}

void trans_CompSt(Node *compst)
{
    // CompSt = LC DefList StmtList RC 函数具体定义，先声明后使用
    trans_DefList(compst->child->next);
    trans_StmtList(compst->child->next->next);
}

void trans_ExtDecList(Node *k)
{
    // ExtDecList = VarDec | VarDec COMMA ExtDecList
    for (Node *i = k;; i = i->child->next->next)
    {
        trans_Dec(k);
        if (i->child->next == NULL)
            break;
    }
}

void trans_Dec(Node *k)
{
    // Dec = VarDec | VarDec ASSIGNOP Exp
    ListNode *p = trans_VarDec(k->child);
    p->para_no = para_count++; //记录这是第几个参数
    Operand *tmp = new_para();
    tmp->value = p->para_no;
    if (p->type->kind == ARRAY || p->type->kind == STRUCTURE)
    {
        Operand *size = new_constant(size_of(p->type));
        new_intercode(DEC_in, NULL, tmp, size, NULL);
    }
    if (k->child->next && strcmp(k->child->next->name, "ASSIGNOP") == 0)
    {
        Operand *tmp2 = trans_exp(k->child->next->next);
        new_intercode(ASSIGN_in, NULL, tmp, tmp2, NULL);
    }
}

ListNode *trans_VarDec(Node *k)
{
    // VarDec = ID | VarDec LB INT RB
    if (strcmp(k->child->name, "ID") == 0)
    { // ID头
        ListNode *now = search_all_listnode(define, k->child->val.char_val, false);
        return now; //数组
    }
    else
        return trans_VarDec(k->child);
}

void trans_stmt(Node *k)
{
    // Stmt = Exp SEMI| CompSt| RETURN Exp SEMI | IF LP Exp RP Stmt
    //|IF LP Exp RP Stmt ELSE Stmt | WHILE LP Exp RP Stmt //使用
    if (k->child_num == 7)
    { // IF LP Exp RP Stmt ELSE Stmt
#ifdef DEBUG
        printf("[IF ELSE]\n");
#endif
        Operand *true_label = new_label();
        Operand *false_label = new_label();
        Operand *tail = new_label();
        trans_cond(k->child->next->next, true_label, false_label);
        new_intercode(LABEL_in, NULL, true_label, NULL, NULL);
        trans_stmt(k->child->next->next->next->next);
        new_intercode(GOTO_in, NULL, tail, NULL, NULL);
        new_intercode(LABEL_in, NULL, false_label, NULL, NULL);
        trans_stmt(k->child->next->next->next->next->next->next);
        new_intercode(LABEL_in, NULL, tail, NULL, NULL);
    }
    else if (k->child_num == 5)
    {
        // IF LP Exp RP Stmt | WHILE LP Exp RP Stmt
        if (strcmp(k->child->name, "IF") == 0)
        {
#ifdef DEBUG
            printf("[IF]\n");
#endif
            Operand *true_label = new_label();
            Operand *false_label = new_label();
            trans_cond(k->child->next->next, true_label, false_label);
            new_intercode(LABEL_in, NULL, true_label, NULL, NULL);
            trans_stmt(k->child->next->next->next->next);
            new_intercode(LABEL_in, NULL, false_label, NULL, NULL);
        }
        else
        {
            Operand *true_label = new_label();
            Operand *false_label = new_label();
            Operand *start = new_label();
            new_intercode(LABEL_in, NULL, start, NULL, NULL);
            trans_cond(k->child->next->next, true_label, false_label);
            new_intercode(LABEL_in, NULL, true_label, NULL, NULL);
            trans_stmt(k->child->next->next->next->next);
            new_intercode(GOTO_in, NULL, start, NULL, NULL);
            new_intercode(LABEL_in, NULL, false_label, NULL, NULL);
        }
    }
    else if (k->child_num == 3)
    {
        // RETURN Exp SEMI
        Operand *exp = trans_exp(k->child->next);
        new_intercode(RETURN_in, NULL, exp, NULL, NULL);
    }
    else if (k->child_num == 2)
        // Exp SEMI
        trans_exp(k->child);
    else
    {
        stack_push();
        stack_change(k->child->symbol);
        trans_CompSt(k->child);
        stack_pop();
    }
}

Operand *trans_exp(Node *k) // if_right表示是否是右值
{                           //返回的是一个操作符（临时变量）
    // Exp = Exp ASSIGNOP Exp| Exp AND Exp | Exp OR Exp
    //| Exp RELOP Exp | Exp PLUS Exp | Exp MINUS Exp
    //| Exp STAR Exp | Exp DIV Exp | LP Exp RP | MINUS Exp
    //| NOT Exp | ID LP Args RP | ID LP RP | Exp LB Exp RB
    //| Exp DOT ID | ID | INT | FLOAT
    Operand *now = NULL;
    if (k->child_num == 1)
    { // ID | INT | FLOAT
        if (strcmp(k->child->name, "INT") == 0)
        {
            now = new_constant(k->child->val.int_val);
#ifdef DEBUG
            printf("[INT %d]\n", now->value);
#endif
        }
        else if (strcmp(k->child->name, "FLOAT") == 0)
        {
            //好像这次没有浮点数
            now = zero;
#ifdef DEBUG
            printf("[ERROR: FLOAT %f]\n", k->child->val.float_val);
#endif
        }
        else if (strcmp(k->child->name, "ID") == 0)
        {
            //不能直接返回名称，因为函数参数名已重置
            ListNode *tmp = search_all_listnode(define,
                                                k->child->val.char_val, false);
#ifdef DEBUG
            printf("[ID %s]\n", now->name);
#endif
            now = new_para();
            now->value = tmp->para_no;
            now->variable = tmp->type; //附加变量
            if (now->variable->kind == BASIC || tmp->para_no <= 0)
                now->type = Normal; //获得的是变量的地址
            else
                now->type = Address;
        }
    }
    else if (k->child_num == 2)
    {
        // NOT Exp | MINUS Exp
        if (strcmp(k->child->name, "NOT") == 0)
        {
            now = new_tmp();
            // x=-y -> x=0-y
            Operand *true_label = new_label();
            Operand *false_label = new_label();
            new_intercode(ASSIGN_in, NULL, now, zero, NULL);
            trans_cond(k, true_label, false_label);
            new_intercode(LABEL_in, NULL, true_label, NULL, NULL);
            new_intercode(ASSIGN_in, NULL, now, one, NULL);
            new_intercode(LABEL_in, NULL, false_label, NULL, NULL);
        }
        else
        {
            if (strcmp(k->child->next->child->name, "INT") == 0)
            {
                //特殊情况：MINUS INT 负数
                now = new_constant(-k->child->next->child->val.int_val);
            }
            else
            {
                Operand *exp = trans_exp(k->child->next);
                // x=-y -> x=0-y
                now = new_tmp();
                new_intercode(MINUS_in, now, zero, exp, NULL);
            }
        }
    }
    else if (k->child_num == 3)
    { // Exp ASSIGNOP Exp | Exp AND Exp | Exp OR Exp
      //| Exp RELOP Exp | Exp PLUS Exp | Exp MINUS Exp
      //| Exp STAR Exp | Exp DIV Exp | LP Exp RP
      //| ID LP RP | Exp DOT ID
        Operand *tmp1 = NULL, *tmp2 = NULL, *tmp3 = NULL;
        if (strcmp(k->child->next->name, "ASSIGNOP") == 0)
        { // Exp ASSIGNOP Exp
            tmp1 = trans_exp(k->child);
            tmp3 = trans_exp(k->child->next->next);
            now = tmp1;
            new_intercode(ASSIGN_in, NULL, now, tmp3, NULL);
        }
        else if (strcmp(k->child->next->name, "PLUS") == 0 ||
                 strcmp(k->child->next->name, "MINUS") == 0 ||
                 strcmp(k->child->next->name, "STAR") == 0 ||
                 strcmp(k->child->next->name, "DIV") == 0)
        { //四则运算
            tmp1 = trans_exp(k->child);
            tmp3 = trans_exp(k->child->next->next);
            now = new_tmp();
            if (strcmp(k->child->next->name, "PLUS") == 0)
                new_intercode(PLUS_in, now, tmp1, tmp3, NULL);
            else if (strcmp(k->child->next->name, "MINUS") == 0)
                new_intercode(MINUS_in, now, tmp1, tmp3, NULL);
            else if (strcmp(k->child->next->name, "STAR") == 0)
                new_intercode(STAR_in, now, tmp1, tmp3, NULL);
            else if (strcmp(k->child->next->name, "DIV") == 0)
                new_intercode(DIV_in, now, tmp1, tmp3, NULL);
        }
        else if (strcmp(k->child->name, "ID") == 0)
        { // ID LP RP
            now = new_tmp();
            Operand *func = new_function(k->child->val.char_val);
            new_intercode(CALL_in, NULL, now, func, NULL);
        }
        else if (strcmp(k->child->name, "LP") == 0)
        { // LP Exp RP
            tmp2 = trans_exp(k->child->next);
            now = tmp2;
        }
        else if (strcmp(k->child->next->name, "DOT") == 0)
        {                                        // Exp DOT ID
            Operand *stru = trans_exp(k->child); //获取前面的结构体
            Operand *tmp = new_tmp();
            new_intercode(ASSIGN_in, NULL, tmp, stru, NULL); //获取结构体的地址
            char *name = k->child->next->next->val.char_val; //成员名字
            int size = 0;
            for (ListNode *i = stru->variable->structure; i; i = i->next)
            {
                //遍历所有成员
                if (strcmp(i->name, name) == 0)
                {
                    new_intercode(PLUS_in, tmp, tmp, new_constant(size), NULL);
                    tmp->variable = i->type; //要附加变量！
                    break;
                }
                size += size_of(i->type);
            }
            now = copy_operand(tmp);
            if (now->variable->kind == ARRAY || now->variable->kind == STRUCTURE)
                now->type = Normal;
            else
                now->type = Star;
        }
        else
        {
            // Exp RELOP Exp | Exp AND Exp | Exp OR Exp
            now = new_tmp();
            // x=-y -> x=0-y
            Operand *true_label = new_label();
            Operand *false_label = new_label();
            new_intercode(ASSIGN_in, NULL, now, zero, NULL);
            trans_cond(k, true_label, false_label);
            new_intercode(LABEL_in, NULL, true_label, NULL, NULL);
            new_intercode(ASSIGN_in, NULL, now, one, NULL);
            new_intercode(LABEL_in, NULL, false_label, NULL, NULL);
        }
    }
    else
    { // ID LP Args RP | Exp LB Exp RB
        if (strcmp(k->child->next->next->name, "Args") == 0)
        {
            Operand *func = new_function(k->child->val.char_val);
            if (strcmp(func->name, "write") == 0)
            {
                Operand *arg = trans_exp(k->child->next->next->child);
                new_intercode(CALL_in, NULL, arg, func, NULL);
            }
            else
            {
                Operand *arg = trans_arg(k->child->next->next);
                now = new_tmp();
                new_intercode(CALL_in, NULL, now, func, NULL);
            }
        }
        else if (strcmp(k->child->next->name, "LB") == 0)
        {
            // Exp LB Exp RB
            Operand *Array = trans_exp(k->child);
            //获取第一个exp对应的数组
            Operand *para = trans_exp(k->child->next->next);
            //获取数组参数
            Operand *tmp = new_tmp(); //记录乘积
            now = new_tmp();
            now->variable = Array->variable->array.elem;
            //处理这一节数组
            new_intercode(STAR_in, tmp, para,
                          new_constant(size_of(now->variable)),
                          NULL); //这一个维度的宽度
            new_intercode(PLUS_in, now, Array, tmp, NULL);
            //前面的地址加上这一维度的空间
            if (now->variable->array.size == 0)
            {
                //最后一个维度，可能也是第一个
                Operand *addr = copy_operand(now);
                now = addr; //最后再变回取值
                if (now->variable->kind == ARRAY || now->variable->kind == STRUCTURE)
                    now->type = Normal;
                //注意如果成员仍是数组或结构体，要继续计算地址，因此不取值
                else
                    now->type = Star;
            }
        }
    }
    return now;
}

Operand *trans_arg(Node *k)
{
    // Args = Exp COMMA Args | Exp
    if (k->child_num > 1)
        trans_arg(k->child->next->next);
    Operand *args = trans_exp(k->child);
    Operand *tmp = copy_operand(args);
    tmp->type = Normal; //传入的都是名字
    new_intercode(ARG_in, NULL, tmp, NULL, NULL);
    return args;
}

void trans_cond(Node *k, Operand *true_label, Operand *false_label)
{
    // Exp AND Exp | Exp OR Exp | Exp RELOP Exp | NOT Exp | others
    if (k->child_num == 3 && strcmp(k->child->next->name, "RELOP") == 0)
    {
        Operand *relop = new_function(k->child->next->val.char_val);
#ifdef DEBUG
        printf("[RELOP %s]\n", relop->name);
#endif
        Operand *exp1 = trans_exp(k->child);
        Operand *exp2 = trans_exp(k->child->next->next);
        new_intercode(IF_in, relop, exp1, exp2, true_label);
        new_intercode(GOTO_in, NULL, false_label, NULL, NULL);
    }
    else if (k->child_num == 3 && strcmp(k->child->next->name, "AND") == 0)
    {
        Operand *true_1 = new_label();
        trans_cond(k->child, true_1, false_label);
        new_intercode(LABEL_in, NULL, true_1, NULL, NULL);
        trans_cond(k->child->next->next, true_label, false_label);
    }
    else if (k->child_num == 3 && strcmp(k->child->next->name, "OR") == 0)
    {
        Operand *false_1 = new_label();
        trans_cond(k->child, true_label, false_1);
        new_intercode(LABEL_in, NULL, false_1, NULL, NULL);
        trans_cond(k->child->next->next, true_label, false_label);
    }
    else if (k->child_num == 2 && strcmp(k->child->name, "NOT") == 0)
        trans_cond(k->child->next, false_label, true_label);
    else
    {
        Operand *exp = trans_exp(k);
        Operand *relop = new_function("!=");
        new_intercode(IF_in, relop, exp, zero, true_label);
        new_intercode(GOTO_in, NULL, false_label, NULL, NULL);
    }
}

void trans_read(Node *k)
{
    if (strcmp(k->name, "Program") == 0)
        stack_change(k->symbol); //添加全局变量
    if (k == NULL)
        return;
    //    printf("now read %s\n", k->name);
    if (strcmp(k->name, "ExtDef") == 0)
        trans_ExtDef(k);
    else
        for (Node *i = k->child; i; i = i->next)
        {
            trans_read(i);
        }
}

Operand *copy_operand(Operand *k)
{
    Operand *tmp = new_tmp();
    tmp->kind = k->kind;
    strcpy(tmp->name, k->name);
    tmp->type = k->type;
    tmp->value = k->value;
    tmp->variable = k->variable;
    return tmp;
}

char *para_name(int k)
{
    char *name = (char *)malloc(sizeof(char) * 40);
    name[0] = 'v';
    sprintf(&name[1], "%d", k);
    return name;
}

void reverse_relop(char *name)
{
    //判断符的翻转
    if (strcmp(name, "!=") == 0)
        strcpy(name, "==");
    else if (strcmp(name, "==") == 0)
        strcpy(name, "!=");
    else if (strcmp(name, "<") == 0)
        strcpy(name, ">=");
    else if (strcmp(name, "<=") == 0)
        strcpy(name, ">");
    else if (strcmp(name, ">") == 0)
        strcpy(name, "<=");
    else if (strcmp(name, ">=") == 0)
        strcpy(name, "<");
}

void clear()
{
    for (InterCode *p = intercode_head; p; p = p->next)
    {
        if (p->kind == RETURN_in)
        {
            for (InterCode *tmp = p->next; tmp; tmp = tmp->next)
            {
                if (tmp->kind != LABEL_in && tmp->kind != FUNCTION_in)
                    remove_intercode(tmp);
                else
                    break;
            }
        } //在return和(label或fuction)之间的语句不可能被访问

        if (p->next != NULL && p->next->kind == LABEL_in)
        {
            if (p->kind == GOTO_in)
            {
                if (p->singop.op->value == p->next->singop.op->value)
                    remove_intercode(p);
            }
            if (p->kind == IF_in)
            {
                if (p->recop.op3->value == p->next->singop.op->value)
                    remove_intercode(p);
            }
        } //利用指令自然流动去掉与对应LABEL相邻的GOTO

        if (p->next != NULL && p->next->next != NULL &&
            p->next->kind == GOTO_in && p->next->next->kind == LABEL_in)
        {
            if (p->kind == IF_in)
            {
                if (p->recop.op3->value == p->next->next->singop.op->value)
                {
                    reverse_relop(p->recop.relop->name);
                    p->recop.op3->value = p->next->singop.op->value;
                    remove_intercode(p->next);
                    remove_intercode(p->next);
                }
            }
        }
        // if x relop y goto label1
        //  goto label2
        //  label1  一类的语句
        //改为if x !relop y goto label2

        if (p->para_num == 3 && p->triop.op1->kind == CONSTANT_operand &&
            p->triop.op2->kind == CONSTANT_operand)
        {
            int tmp1 = p->triop.op1->value, tmp2 = p->triop.op2->value, tmp3;
            if (p->kind == PLUS_in)
                tmp3 = tmp1 + tmp2;
            else if (p->kind == MINUS_in)
                tmp3 = tmp1 - tmp2;
            else if (p->kind == STAR_in)
                tmp3 = tmp1 * tmp2;
            else if (p->kind == DIV_in)
                tmp3 = tmp1 / tmp2; //计算常数结果
            //重建一个常数赋值
            p->kind = ASSIGN_in;
            p->para_num = 2;
            p->binop.op1 = p->triop.result;
            p->binop.op2 = new_constant(tmp3);
            //           *(p->triop.result) = *(new_constant(tmp3));
            //这一步直接改变了该操作数指针指向的操作数的值
            //因此所有引用该操作数的指令都会同时改变
            //           remove_intercode(p);
        } //常数之间的运算可以直接赋值
    }
}

void clear2()
{
    for (InterCode *p = intercode_head; p; p = p->next)
    {
        if (p->kind == LABEL_in)
        {
            int tmp = 0;
            for (InterCode *i = intercode_head; i; i = i->next)
            {
                //重新遍历，看该label被几个goto饮用
                if (i->kind == GOTO_in)
                {
                    if (i->singop.op->value == p->singop.op->value)
                        tmp++;
                }
                if (i->kind == IF_in)
                {
                    if (i->recop.op3->value == p->singop.op->value)
                        tmp++;
                }
            }
            if (tmp == 0)
                remove_intercode(p);
            //如果没有goto引用它，就可以将其去掉
        } //清除无用的label
        if (p->para_num == 3)
        {
            if (p->kind == PLUS_in)
            {
                if ((p->triop.result == p->triop.op2 &&
                     p->triop.op1->kind == CONSTANT_operand &&
                     p->triop.op1->value == 0) ||
                    (p->triop.result == p->triop.op1 &&
                     p->triop.op2->kind == CONSTANT_operand &&
                     p->triop.op2->value == 0))
                    remove_intercode(p); //加0等于自己
            }
            else if (p->kind == STAR_in)
            {
                if ((p->triop.result == p->triop.op2 &&
                     p->triop.op1->kind == CONSTANT_operand &&
                     p->triop.op1->value == 1) ||
                    (p->triop.result == p->triop.op1 &&
                     p->triop.op2->kind == CONSTANT_operand &&
                     p->triop.op2->value == 1))
                    remove_intercode(p); //乘1等于自己
            }
        }
    }
}