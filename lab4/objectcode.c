#include "objectcode.h"

#define DEBUG

//对每个函数而言，最上层是参数，然后fp指向自己的旧值，接下来是各个新定义的变量

Reg_descriper reg[32];    // 32个寄存器
int fp_offset = 0;        //记录当前层离fp有多远
Operand *var_list = NULL; //对于*x类的变量，需要把它关联到x，所以用该变量记录所有x
extern InterCode *intercode_head;

void init_reg()
{
    for (int i = 0; i < 32; i++)
    {
        reg[i].var_store = NULL;
        reg[i].age = 0;
    }
    strcpy(reg[0].name, "$zero");
    strcpy(reg[1].name, "$at");
    strcpy(reg[2].name, "$v0");
    strcpy(reg[3].name, "$v1");
    strcpy(reg[4].name, "$a0");
    strcpy(reg[5].name, "$a1");
    strcpy(reg[6].name, "$a2");
    strcpy(reg[7].name, "$a3");

    strcpy(reg[8].name, "$t0");
    strcpy(reg[9].name, "$t1");
    strcpy(reg[10].name, "$t2");
    strcpy(reg[11].name, "$t3");
    strcpy(reg[12].name, "$t4");
    strcpy(reg[13].name, "$t5");
    strcpy(reg[14].name, "$t6");
    strcpy(reg[15].name, "$t7");

    strcpy(reg[16].name, "$s0");
    strcpy(reg[17].name, "$s1");
    strcpy(reg[18].name, "$s2");
    strcpy(reg[19].name, "$s3");
    strcpy(reg[20].name, "$s4");
    strcpy(reg[21].name, "$s5");
    strcpy(reg[22].name, "$s6");
    strcpy(reg[23].name, "$s7");

    strcpy(reg[24].name, "$t8");
    strcpy(reg[25].name, "$t9");
    strcpy(reg[26].name, "$k0");
    strcpy(reg[27].name, "$k1");
    strcpy(reg[28].name, "$gp");
    strcpy(reg[29].name, "$sp"); // ESP
    strcpy(reg[30].name, "$fp"); // EBP
    strcpy(reg[31].name, "$ra");
    //可用寄存器：t0-t9,s0-s7
}

void add_op(Operand *x)
{
    x->next = var_list;
    var_list = x;
}

int find_op_offset(Operand *x)
{
    if (x->offset != -1)
        return x->offset;
    //否则在参数变量表里找
    Operand *now = var_list;
    while (now)
    {
        if (op_equal(x, now))
        {
            x->offset = now->offset;
            return x->offset;
        }
        now = now->next;
    }
    return -1;
}

void push_op(Operand *x)
{
    if (x->kind != PARA_operand && x->kind != TMP_operand)
        return;
    fp_offset += 4;
    x->offset = fp_offset; //记录变量的位置
#ifdef DEBUG
    printf("    [op ");
    print_operand(x);
    printf(" offset = %d]\n", x->offset);
#endif
    add_op(x);
}

void save_all_reg()
{
#ifdef DEBUG
    printf("    [save all regs]\n");
#endif
    for (int i = 8; i <= 25; i++)
    {
        Operand *now = reg[i].var_store;
        if (now == NULL)
            continue;
        if (now->kind == PARA_operand || now->kind == TMP_operand)
            printf("sw %s, -%d($fp)\n", reg[i].name, now->offset); //变量值存回栈
        reg[i].var_store = NULL;
        reg[i].age = 0;
    }
}

int get_reg(Operand *x)
{
    //首先检查是否已入栈（参数和变量需要）
    if (find_op_offset(x) == -1)
        push_op(x);

    //更新寿命
    for (int i = 8; i <= 25; i++)
    {
        Operand *now = reg[i].var_store; //注意寄存器不止存一个Op
        if (now != NULL)
            reg[i].age++;
    }

    //先找是否在现有寄存器里
    int target = -1;
    for (int i = 8; i <= 25; i++)
    {
        Operand *now = reg[i].var_store;
        if (now == NULL)
        {
            target = i;
            break;
        }
        //如果该寄存器没有存值，直接选择它
        if (op_equal(now, x))
        {
#ifdef DEBUG
            printf("    [");
            print_operand(x);
            printf(" -> reg %d %s]\n", i, reg[i].name);
#endif
            return i;
        }
    } //如果已在则直接返回

    //若不在寄存器中则找最不常使用的替换掉
    int oldest = -1, oldage = -1;
    if (target == -1)
    {
        for (int i = 8; i <= 25; i++)
        {
            if (reg[i].age > oldage)
            {
                oldage = reg[i].age;
                oldest = i;
            }
        }
        //返回应该被替换的编号
        target = oldest;
        Operand *now = reg[oldest].var_store; //注意寄存器不止存一个Op
        if (now->kind == PARA_operand || now->kind == TMP_operand)
            printf("sw %s, -%d($fp)\n", reg[oldest].name, now->offset);
        //把里面的存回栈，注意只有参数和变量需要
    }

    reg[target].age = 0;
    reg[target].var_store = x;
    if (x->kind == PARA_operand || x->kind == TMP_operand)
        printf("lw %s, -%d($fp)\n", reg[target].name, x->offset);
    else
        printf("li %s, %d\n", reg[target].name, x->value);

#ifdef DEBUG
    printf("    [");
    print_operand(x);
    printf(" -> reg %d %s]\n", target, reg[target].name);
#endif
    return target;
}

bool op_equal(Operand *x, Operand *y)
{
    if (x == NULL || y == NULL)
        return false;
    if (x->kind == y->kind && x->value == y->value)
        return true;
    return false;
}

void object_read()
{
    InterCode *p = intercode_head;
    while (p != NULL)
    { //解析每条指令
        if (p->kind == LABEL_in)
        {
            printf("LABEL%d:\n", p->singop.op->value);
        }
        else if (p->kind == FUNCTION_in)
        {
            printf("FUNCTION_");
            print_operand(p->singop.op);
            printf(":\n");
            // push ebp
            printf("addi $sp, $sp, -4\n");
            printf("sw $fp, 0($sp)\n");
            // ebp=esp
            printf("move $fp, $sp\n");
            //获取参数
            p = p->next;
            int offset = p->singop.op->value * 4;
#ifdef DEBUG
            printf("paranum = %d\n", offset / 4);
#endif
            while (p->kind == PARAM_in)
            {
                Operand *op = p->singop.op;
                int reg1 = get_reg(op);
                printf("lw %s, %d($fp)\n", reg[reg1].name, offset);
                offset -= 4;
                p = p->next;
            }
            continue;
        }
        else if (p->kind == GOTO_in)
        {
            printf("j LABEL%d\n", p->singop.op->value);
            save_all_reg(); //一个基本块的结束
        }
        else if (p->kind == RETURN_in)
        {

            save_all_reg(); //一个基本块的结束
        }
        else if (p->kind == ARG_in)
        { // sp负责压入参数
            int i = 0;
            while (p->kind == ARG_in)
            {
                i++;
                int reg1 = get_reg(p->singop.op);
                printf("sw %s, -%d($sp)\n", reg[reg1].name, i * 4);
                p = p->next;
            }
            printf("addi $sp, $sp, -%d\n", 4 * i);
            continue;
        }
        else if (p->kind == PARAM_in)
        {
        }
        else if (p->kind == READ_in)
        {
        }
        else if (p->kind == WRITE_in)
        {
        }
        else if (p->kind == ASSIGN_in)
        {
            Operand *op1 = p->binop.op1;
            Operand *op2 = p->binop.op2;
            int reg1 = get_reg(op1); //* []
            int reg2 = get_reg(op2); //* & [] #
            if (op1->type == Star)
            { //*x= [] #
                printf("sw %s, 0(%s)\n", reg[reg2].name, reg[reg1].name);
            }
            else
            { // x= * & [] #
                if (op2->type == Star)
                    printf("lw %s, 0(%s)\n", reg[reg1].name, reg[reg2].name);
                else
                    printf("move %s, %s\n", reg[reg1].name, reg[reg2].name);
            }
        }
        else if (p->kind == DEC_in)
        {
        }
        else if (p->kind == CALL_in)
        {

            save_all_reg(); //一个基本块的结束
        }
        else if (p->para_num == 3)
        { //三个操作数，加减乘除
            Operand *result = p->triop.result;
            Operand *op1 = p->triop.op1;
            Operand *op2 = p->triop.op2;
            int reg3 = get_reg(result);
            int reg1 = get_reg(op1);
            int reg2 = get_reg(op2);
            if (p->kind == PLUS_in)
            {
                //有常数
                if (op1->kind == CONSTANT_operand)
                    printf("addi %s, %s, %d\n", reg[reg3].name,
                           reg[reg2].name, op1->value);
                else if (op2->kind == CONSTANT_operand)
                    printf("addi %s, %s, %d\n", reg[reg3].name,
                           reg[reg1].name, op2->value);
                //无常数
                else
                    printf("add %s, %s, %s\n", reg[reg3].name,
                           reg[reg1].name, reg[reg2].name);
            }
            else if (p->kind == MINUS_in)
            {
                //有常数
                if (op1->kind == CONSTANT_operand)
                    printf("addi %s, %s, %d\n", reg[reg3].name,
                           reg[reg2].name, -op1->value);
                else if (op2->kind == CONSTANT_operand)
                    printf("addi %s, %s, %d\n", reg[reg3].name,
                           reg[reg1].name, -op2->value);
                //无常数
                else
                    printf("sub %s, %s, %s\n", reg[reg3].name,
                           reg[reg1].name, reg[reg2].name);
            }
            else if (p->kind == STAR_in)
            {
                printf("mul %s, %s, %s\n", reg[reg3].name,
                       reg[reg1].name, reg[reg2].name);
            }
            else if (p->kind == DIV_in)
            {
                printf("div %s, %s\n", reg[reg1].name, reg[reg2].name);
                printf("mflo %s\n", reg[reg3].name);
            }
        }
        else if (p->kind == IF_in)
        {
            int reg1 = get_reg(p->recop.op1);
            int reg2 = get_reg(p->recop.op2);
            save_all_reg(); //一个基本块的结束

            if (strcmp(p->recop.relop->name, "=="))
                printf("beq ");
            else if (strcmp(p->recop.relop->name, "!="))
                printf("bne ");
            else if (strcmp(p->recop.relop->name, ">"))
                printf("bgt ");
            else if (strcmp(p->recop.relop->name, "<"))
                printf("blt ");
            else if (strcmp(p->recop.relop->name, ">="))
                printf("bge ");
            else if (strcmp(p->recop.relop->name, "<="))
                printf("ble ");
            printf("%s, %s, LABEL%d\n", reg[reg1].name,
                   reg[reg2].name, p->recop.op3->value);
        }
        p = p->next;
    }
}

void create_code()
{
    init_reg();

    object_read();
}