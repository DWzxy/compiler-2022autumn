#include "intermediate.h"
#include "semantic.h"

//#define DEBUG
#define SIZE 4

InterCode *intercode_head = NULL;
int label_count = 0; //标号数目
int tmp_count = 0;   //临时变量数目
int para_count = 0;  //函数参数数目

void trans_init()
{
}

void insert_intercode(InterCode *k)
{
    if (intercode_head == NULL)
        intercode_head = k;
    else
    {
        intercode_head->pre = NULL;
        k->next = intercode_head;
        intercode_head = k;
    }
    print_intercode(k);
}

Operand *new_operand(enum Operand_kind kind, int type, char *name)
{
    Operand *p = (Operand *)malloc(sizeof(Operand));
    strcpy(p->name, "");
    p->kind = kind;
    p->type = type;
    if (kind == VARIABLE_operand)
    {
        strcpy(p->name, name);
    }
    else if (kind == CONSTANT_operand)
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
    }
    else if (kind == ASSIGN_in || kind == DEC_in || kind == CALL_in)
    {
        p->binop.op1 = op1;
        p->binop.op2 = op2;
    }
    else if (kind == PLUS_in || kind == MINUS_in || kind == STAR_in || kind == DIV_in)
    {
        p->triop.op1 = op1;
        p->triop.op2 = op2;
        p->triop.result = res;
    }
    else if (kind == IF_in)
    {
        p->recop.op1 = op1;
        p->recop.op2 = op2;
        p->recop.op3 = op3;
        p->recop.relop = res;
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

Operand *new_constant()
{
    return new_operand(CONSTANT_operand, Normal, "");
}

Operand *new_varible(char *name)
{
    return new_operand(VARIABLE_operand, Normal, name);
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
    if (kind == VARIABLE_operand || kind == FUNCTION_operand)
        printf("%s", k->name);
    else if (kind == CONSTANT_operand)
        printf("#%d", k->value);
    else if (kind == TMP_operand)
        printf("tmp%d", k->value);
    else if (kind == PARA_operand)
        printf("v%d", k->value);
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
        printf(" ");
        print_operand(k->binop.op2);
    }
    else if (kind == CALL_in)
    {
        print_operand(k->binop.op1);
        printf(" := CALL ");
        print_operand(k->binop.op2);
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
        trans_FunDec(k->child->next); // push在此
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
    stack_push();
    if (k->child_num == 4)
    { //有参数
        trans_VarList(k->child->next->next);
    }
    return;
}

void trans_VarList(Node *k)
{
    // ParamDec COMMA VarList| ParamDec
    for (Node *i = k->child;; i = i->next->next->child)
    {
        //每一个参数定义
        // Specifier VarDec
        ListNode *p = trans_VarDec(i->child->next);
        p->para_no = para_count++; //记录这是第几个参数
        insert_listnode(p, &define_stack[define_top]);

        Operand *param = new_para();
        param->value = p->para_no;
        new_intercode(PARAM_in, NULL, param, NULL, NULL);
        if (i->next == NULL)
            break;
    }
    return;
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
    insert_listnode(p, &define_stack[define_top]);
    Operand *name = new_varible(p->name);
    if (p->type->kind == ARRAY || p->type->kind == STRUCTURE)
    {
        Operand *size = new_constant();
        size->value = size_of(p->type);
        new_intercode(DEC_in, NULL, name, size, NULL);
    }
    if (k->child->next && strcmp(k->child->next->name, "ASSIGNOP") == 0)
    {
        Operand *tmp2 = trans_exp(k->child->next->next);
        new_intercode(ASSIGN_in, NULL, name, tmp2, NULL);
    }
}

ListNode *trans_VarDec(Node *k)
{
    // VarDec = ID | VarDec LB INT RB
    if (strcmp(k->child->name, "ID") == 0)
    { // ID头
        Type *type = new_type(BASIC);
        ListNode *now = new_listnode(k->child->val.char_val, type);
        return now; //数组
    }
    else
    {                                         //数组后半部分
        ListNode *p = trans_VarDec(k->child); //头节点，也就是TYPE
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
        trans_CompSt(k->child);
    }
}

Operand *trans_exp(Node *k)
{ //返回的是一个操作符（临时变量）
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
            now = new_constant();
            now->value = k->child->val.int_val;
#ifdef DEBUG
            printf("[INT %d]\n", now->value);
#endif
        }
        else if (strcmp(k->child->name, "FLOAT") == 0)
        {
            //好像这次没有浮点数
            now = new_constant();
            now->value = 0;
#ifdef DEBUG
            printf("[ERROR: FLOAT %f]\n", k->child->val.float_val);
#endif
        }
        else if (strcmp(k->child->name, "ID") == 0)
        {
            //不能直接返回名称，因为函数参数名已重置
            ListNode *tmp = search_all_listnode(define,
                                                k->child->val.char_val, false);
            if (tmp->para_no == -1)
            {
                now = new_varible(k->child->val.char_val);
#ifdef DEBUG
                printf("[ID %s]\n", now->name);
#endif
            }
            else
            {
                now = new_para();
                now->value = tmp->para_no;
            }
        }
    }
    else if (k->child_num == 2)
    {
        // NOT Exp | MINUS Exp
        if (strcmp(k->child->name, "NOT") == 0)
        {
        }
        else
        {
            Operand *exp = trans_exp(k->child->next);
            Operand *zero = new_constant();
            zero->value = 0; // x=-y -> x=0-y
            now = new_tmp();
            new_intercode(MINUS_in, now, zero, exp, NULL);
        }
    }
    else if (k->child_num == 3)
    { // Exp ASSIGNOP Exp | Exp AND Exp | Exp OR Exp
      //| Exp RELOP Exp | Exp PLUS Exp | Exp MINUS Exp
      //| Exp STAR Exp | Exp DIV Exp | LP Exp RP
      //| ID LP RP | Exp DOT ID
        Operand *tmp1 = NULL, *tmp2 = NULL, *tmp3 = NULL;
        if (strcmp(k->child->name, "Exp") == 0)
            tmp1 = trans_exp(k->child);
        if (strcmp(k->child->next->name, "Exp") == 0)
            tmp2 = trans_exp(k->child->next);
        if (strcmp(k->child->next->next->name, "Exp") == 0)
            tmp3 = trans_exp(k->child->next->next);
        if (strcmp(k->child->next->name, "ASSIGNOP") == 0)
        { // Exp ASSIGNOP Exp
            now = tmp1;
            new_intercode(ASSIGN_in, NULL, now, tmp3, NULL);
        }
        else if (strcmp(k->child->next->name, "PLUS") == 0 ||
                 strcmp(k->child->next->name, "MINUS") == 0 ||
                 strcmp(k->child->next->name, "STAR") == 0 ||
                 strcmp(k->child->next->name, "DIV") == 0)
        { //四则运算
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
            now = tmp2;
        }
        else if (strcmp(k->child->next->name, "DOT") == 0)
        { // Exp DOT ID
        }
        else
        {
            // Exp RELOP Exp | Exp AND Exp | Exp OR Exp
            now = new_tmp();
            Operand *zero = new_constant();
            zero->value = 0; // x=-y -> x=0-y
            Operand *true_label = new_label();
            Operand *false_label = new_label();
            new_intercode(ASSIGN_in, NULL, now, zero, NULL);
            trans_cond(k, true_label, false_label);
            new_intercode(LABEL_in, NULL, true_label, NULL, NULL);
            zero->value = 1;
            new_intercode(ASSIGN_in, NULL, now, zero, NULL);
            new_intercode(LABEL_in, NULL, false_label, NULL, NULL);
        }
    }
    else
    { // ID LP Args RP | Exp LB Exp RB
        if (strcmp(k->child->next->next->name, "Args") == 0)
        {
            trans_arg(k->child->next->next);
            now = new_tmp();
            Operand *func = new_function(k->child->val.char_val);
            new_intercode(CALL_in, NULL, now, func, NULL);
        }
        else if (strcmp(k->child->next->name, "LB") == 0)
        { // Exp LB Exp RB
        }
    }
    return now;
}

void trans_arg(Node *k)
{
    // Args = Exp COMMA Args | Exp
    if (k->child_num > 1)
        trans_arg(k->child->next->next);
    Operand *args = trans_exp(k->child);
    new_intercode(ARG_in, NULL, args, NULL, NULL);
}

void trans_cond(Node *k, Operand *true_label, Operand *false_label)
{
    // Exp AND Exp | Exp OR Exp | Exp RELOP Exp | NOT Exp | others
    if (k->child_num == 3 && strcmp(k->child->next->name, "RELOP") == 0)
    {
        Operand *relop = new_varible(k->child->next->val.char_val);
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
        trans_cond(k->child, true_label, false_label);
        new_intercode(LABEL_in, NULL, false_1, NULL, NULL);
        trans_cond(k->child->next->next, true_label, false_label);
    }
    else if (k->child_num == 2 && strcmp(k->child->name, "NOT") == 0)
        trans_cond(k->child->next, false_label, true_label);
    else
    {
        Operand *exp = trans_exp(k);
        Operand *relop = new_varible("!=");
        Operand *zero = new_constant();
        new_intercode(IF_in, relop, exp, zero, true_label);
        new_intercode(GOTO_in, NULL, false_label, NULL, NULL);
    }
}

void trans_read(Node *k)
{
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