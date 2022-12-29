#include "intermediate.h"
#include "semantic.h"

// #define DEBUG
#define SIZE 4

InterCode *intercode_head = NULL;
InterCode *intercode_tail = NULL;
int label_count = 1; // 标号数目
int tmp_count = 1;   // 临时变量数目
int para_count = 1;  // 函数参数数目

Operand *zero;
Operand *one;
Operand *size;

int cond_num = 0;

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
    //   print_intercode(k);
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
    // 虽然从链表里去掉了k，但k仍在，可以访问它原来的pre和next
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
        // 只能单独打印，因为DEC中的常数前不加#
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
        return size;
    }
    else if (k->kind == FUNC)
        return 0;
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
    // 判断符的翻转
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

bool operand_equal(Operand *x, Operand *y)
{
    if (x == NULL || y == NULL)
        return false;
    if (x->kind != y->kind)
        return false;
    if (x->type != y->type)
        return false;
    if (x->kind == CONSTANT_operand)
        return x->value == y->value;
    else if (x->kind == FUNCTION_operand)
        return strcmp(x->name, y->name) == 0;
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
        } // 在return和(label或fuction)之间的语句不可能被访问

        else if (p->next != NULL && p->next->kind == LABEL_in)
        {
            if (p->kind == GOTO_in)
            {
                if (operand_equal(p->singop.op, p->next->singop.op))
                    remove_intercode(p);
            }
            if (p->kind == IF_in)
            {
                if (operand_equal(p->recop.op3,
                                  p->next->singop.op))
                    remove_intercode(p);
            }
        } // 利用指令自然流动去掉与对应LABEL相邻的GOTO

        else if (p->kind == IF_in && p->next != NULL && p->next->next != NULL &&
                 p->next->kind == GOTO_in && p->next->next->kind == LABEL_in)
        {
            if (operand_equal(p->recop.op3,
                              p->next->next->singop.op))
            {
                reverse_relop(p->recop.relop->name);
                p->recop.op3 = p->next->singop.op;
                remove_intercode(p->next->next);
                remove_intercode(p->next);
            }
        }
        // if x relop y goto label1
        //  goto label2
        //  label1  一类的语句
        // 改为if x !relop y goto label2
    }
}

void clear1()
{
    for (InterCode *p = intercode_head; p; p = p->next)
    {
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
                tmp3 = tmp1 / tmp2; // 计算常数结果
            // 重建一个常数赋值
            p->kind = ASSIGN_in;
            p->para_num = 2;
            p->binop.op1 = p->triop.result;
            p->binop.op2 = new_constant(tmp3);
            //           *(p->triop.result) = *(new_constant(tmp3));
            // 这一步直接改变了该操作数指针指向的操作数的值
            // 因此所有引用该操作数的指令都会同时改变
            //           remove_intercode(p);
        } // 常数之间的运算可以直接赋值
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
                // 重新遍历，看该label被几个goto饮用
                if (i->kind == GOTO_in)
                {
                    if (operand_equal(i->singop.op, p->singop.op))
                        tmp++;
                }
                if (i->kind == IF_in)
                {
                    if (operand_equal(i->recop.op3, p->singop.op))
                        tmp++;
                }
            }
            if (tmp == 0)
                remove_intercode(p);
            // 如果没有goto引用它，就可以将其去掉
        } // 清除无用的label
        if (p->para_num == 3)
        {
            if (p->kind == PLUS_in)
            {
                if ((operand_equal(p->triop.result, p->triop.op2) &&
                     p->triop.op1->kind == CONSTANT_operand &&
                     p->triop.op1->value == 0) ||
                    (operand_equal(p->triop.result, p->triop.op1) &&
                     p->triop.op2->kind == CONSTANT_operand &&
                     p->triop.op2->value == 0))
                    remove_intercode(p); // 加0等于自己
            }
            else if (p->kind == STAR_in)
            {
                if ((operand_equal(p->triop.result, p->triop.op2) &&
                     p->triop.op1->kind == CONSTANT_operand &&
                     p->triop.op1->value == 1) ||
                    (operand_equal(p->triop.result, p->triop.op1) &&
                     p->triop.op2->kind == CONSTANT_operand &&
                     p->triop.op2->value == 1))
                    remove_intercode(p); // 乘1等于自己
            }
        }
    }
}