#include "objectcode.h"

// #define DEBUG

// 对每个函数而言，最上层是参数，然后fp指向自己的旧值，接下来是各个新定义的变量

Reg_descriper reg[32];    // 32个寄存器
int fp_offset = 0;        // 记录当前层离fp有多远
Operand *var_list = NULL; // 对于*x类的变量，需要把它关联到x，所以用该变量记录所有x
extern InterCode *intercode_head;
extern int cond_num;
/*
arg1
arg2
arg3
ra
old_ebp <-ebp
param1
param2
param3
*/

int loop_count = 0;
void loop()
{
    loop_count++;
    printf("loop%d:\n", loop_count);
    printf("    j loop%d\n", loop_count);
    loop_count++;
}

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
    // 可用寄存器：t0-t9,s0-s7，即8-25号
}

void add_op(Operand *x)
{
    x->next = var_list;
    var_list = x;
}

int para_var_count(InterCode *x)
{
    int now = 0;
    for (InterCode *i = x->next; i != NULL && i->kind != FUNCTION_in; i = i->next)
    {
        now += 4 * 3; // 很宽泛的估计,一句指令最多有3个变量
        if (i->kind == DEC_in)
            now += i->binop.op2->value;
        // 特殊情况:申请了额外空间的变量
    }
    return now;
}

Operand *find_var(Operand *x)
{
    // 在参数变量表里找
    Operand *now = var_list;
    while (now)
    {
        if (op_equal(x, now))
        {
            x->offset = now->offset;
            return now;
        }
        now = now->next;
    }
    return NULL;
}

void push_op(Operand *x)
{
    fp_offset -= 4;        // 数组的空间在DEC语句里处理
    x->offset = fp_offset; // 记录变量的位置
#ifdef DEBUG
    printf("    [op ");
    print_operand(x);
    printf(" offset = %d]\n", x->offset);
#endif
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
        if ((now->kind == PARA_operand || now->kind == TMP_operand) &&
            now->type != Star)
            printf("  sw %s, %d($fp)\n", reg[i].name, now->offset); // 变量值存回栈
        reg[i].var_store = NULL;
        reg[i].age = 0;
    }
}

int get_reg(Operand *x)
{
    bool flag = 0; // 记录是否入栈
                   // 首先检查是否已入栈（参数和变量需要）
    if ((x->kind == PARA_operand || x->kind == TMP_operand) &&
        x->type != Star)
    {
        Operand *tmp = find_var(x);
        if (tmp == NULL && x->offset == -1)
        {
            push_op(x);
            add_op(x);
        }
        else
        {
            flag = 1;
        }
    }

    // 更新寿命
    for (int i = 8; i <= 25; i++)
    {
        Operand *now = reg[i].var_store;
        if (now != NULL)
            reg[i].age++;
    }

    // 先找是否在现有寄存器里
    int target = -1;
    for (int i = 8; i <= 25; i++)
    {
        Operand *now = reg[i].var_store;
        if (op_equal(now, x))
        {
#ifdef DEBUG
            printf("    [");
            print_operand(x);
            printf(" -> existed reg %d %s]\n", i, reg[i].name);
#endif
            reg[i].age = 0; // 关键点！气死我了
            return i;
        }
    } // 如果已在则直接返回

    for (int i = 8; i <= 25; i++)
    {
        if (reg[i].var_store == NULL)
        {
            target = i;
#ifdef DEBUG
            printf("    [");
            print_operand(x);
            printf(" -> free reg %d %s]\n", target, reg[target].name);
#endif
            break;
        }
    } // 若有未用的则直接选择

    // 若不在寄存器中则找最不常使用的替换掉
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
        // 返回应该被替换的编号
        target = oldest;
        Operand *now = reg[oldest].var_store;
        if (now->kind == PARA_operand || now->kind == TMP_operand)
        {
            printf("  sw %s, %d($fp)\n", reg[oldest].name, now->offset);
        }
        // 把里面的存回栈，注意只有参数和变量需要
#ifdef DEBUG
        printf("    [");
        print_operand(x);
        printf(" -> reg %d %s]\n", target, reg[target].name);
#endif
    }

    if (x->kind == CONSTANT_operand)
    { // 常数
        reg[target].age = 0;
        reg[target].var_store = x;
        printf("  li %s, %d\n", reg[target].name, x->value);
    }
    else if (x->type == Address)
    { // 特殊的取值,因为第一次出现&x我们也知道它的值
        reg[target].age = 0;
        reg[target].var_store = x;
        Operand *tmp_x = copy_operand(x);
        tmp_x->type = Normal;
        Operand *offset_1 = find_var(tmp_x);
        // 找到x的偏移量，再算&x
        printf("  addi %s, $fp, %d\n", reg[target].name, offset_1->offset);
    }
    else if (x->type == Star)
    {
        reg[target].age = 0;
        Operand *tmp_x = copy_operand(x);
        tmp_x->type = Normal;
        target = get_reg(tmp_x);
        // 找到x的寄存器
    }
    else if (x->kind == PARA_operand || x->kind == TMP_operand)
    {
        reg[target].age = 0;
        reg[target].var_store = x;
        if (flag == 1)
        { // 第一次入栈不需要取值
            printf("  lw %s, %d($fp)\n", reg[target].name, x->offset);
        }
    }

    return target;
}

void into_func()
{
    fp_offset = 0;   // 重置偏移量
    var_list = NULL; // 变量为空

    // push ebp
    printf("  addi $sp, $sp, -8\n");
    printf("  sw $ra, 4($sp)\n");
    printf("  sw $fp, 0($sp)\n");
    // ebp=esp
    printf("  move $fp, $sp\n");
}

void leave_func()
{
    save_all_reg();               // 一个基本块的结束
    printf("  move $sp, $fp\n");  // esp=ebp
    printf("  lw $fp, 0($sp)\n"); // 恢复ebp旧值
    printf("  lw $ra, 4($sp)\n"); // 恢复ra旧值
    printf("  addi $sp, $sp, 8\n");
    printf("  jr $ra\n");
}

bool op_equal(Operand *x, Operand *y)
{
    if (x == NULL || y == NULL)
        return false;
    if (x->kind == y->kind && x->type == y->type && x->value == y->value)
        return true;
    return false;
}

void object_read()
{
    InterCode *p = intercode_head;
    while (p != NULL)
    { // 解析每条指令
        cond_num++;
        if (p->kind == LABEL_in)
        {
            save_all_reg();
            printf("LABEL%d:\n", p->singop.op->value);
        }
        else if (p->kind == FUNCTION_in)
        {

            printf("\n");
            print_operand(p->singop.op);
            printf(":\n");
            into_func();

            // 获取参数
            int tmp_offset = 1 * 4 + 4;
            // 因为有个ra
            int sp_offset = para_var_count(p);
            printf("  addi $sp, $sp, -%d\n", sp_offset);
            // 给变量预留空间
#ifdef DEBUG
            printf("    [ %s paranum = %d]\n", p->singop.op->name, p->singop.op->value);
#endif
            p = p->next;
            while (p->kind == PARAM_in)
            {
                Operand *op = p->singop.op;
                int reg_num = get_reg(op); // 给参数分配栈空间
                printf("  lw %s, %d($fp)\n", reg[reg_num].name, tmp_offset);
                // 加载参数值
                tmp_offset += 4;
                p = p->next;
            }
            continue;
        }
        else if (p->kind == GOTO_in)
        {
            save_all_reg(); // 一个基本块的结束
            printf("  j LABEL%d\n", p->singop.op->value);
        }
        else if (p->kind == RETURN_in)
        {
            int reg1 = get_reg(p->singop.op);
            printf("  move $v0, %s\n", reg[reg1].name); // 保存返回值
            save_all_reg();
            leave_func();
        }
        else if (p->kind == ARG_in)
        { // sp负责压入参数
            int i = 0;
            while (p->kind == ARG_in)
            {
                i++;
                Operand *x = p->singop.op;
                int target = get_reg(x);
                printf("  sw %s, -%d($sp)\n", reg[target].name, i * 4);
                p = p->next;
            }
            printf("  addi $sp, $sp, -%d\n", 4 * i);
            continue;
        }
        else if (p->kind == PARAM_in)
        {
            // 不应该出现,已在Function处理
        }
        else if (p->kind == READ_in)
        {
            if (p->singop.op->type == Star || p->singop.op->type == Address)
            {
                loop();
            }
            printf("  jal read\n");
            int reg1 = get_reg(p->singop.op);
            printf("  move %s, $v0\n", reg[reg1].name);
        }
        else if (p->kind == WRITE_in)
        {
            int target = get_reg(p->singop.op);
            if (p->singop.op->type == Star || p->singop.op->type == Address)
            {
                loop();
            }
            printf("  move $a0, %s\n", reg[target].name);
            printf("  jal write\n");
        }
        else if (p->kind == ASSIGN_in)
        {
            Operand *op1 = p->binop.op1;
            Operand *op2 = p->binop.op2;
            int reg1 = get_reg(op1); //* []
            int reg2 = get_reg(op2); //* & [] #
            if (op1->type == Star)
            { //*x= []
                if (op2->type == Star || op2->type == Address)
                {
                    loop();
                }
                printf("  sw %s, 0(%s)\n", reg[reg2].name, reg[reg1].name);
            }
            else
            { // x= * & []
                if (op1->type == Address)
                {
                    loop();
                }
                if (op2->type == Star)
                    printf("  lw %s, 0(%s)\n", reg[reg1].name, reg[reg2].name);
                else
                    printf("  move %s, %s\n", reg[reg1].name, reg[reg2].name);
            }
        }
        else if (p->kind == DEC_in)
        {
            get_reg(p->binop.op1);
            fp_offset -= p->binop.op2->value - 4;
            p->binop.op1->offset = fp_offset;
#ifdef DEBUG
            printf("    [op ");
            print_operand(p->binop.op1);
            printf(" offset = %d]\n", p->binop.op1->offset);
#endif
        }
        else if (p->kind == CALL_in)
        {
            save_all_reg(); // 一个基本块的结束
            printf("  jal %s\n", p->binop.op2->name);
            // 把返回值从v0里取出来
            int reg1 = get_reg(p->binop.op1);
            printf("  move %s, $v0\n", reg[reg1].name);
        }
        else if (p->para_num == 3)
        { // 三个操作数，加减乘除
            Operand *result = p->triop.result;
            Operand *op1 = p->triop.op1;
            Operand *op2 = p->triop.op2;
            int reg3 = get_reg(result);
            int reg1 = get_reg(op1);
            int reg2 = get_reg(op2);
            if (p->kind == PLUS_in)
            {
                // 有常数
                /*       if (op1->kind == CONSTANT_operand)
                           printf("  addi %s, %s, %d\n", reg[reg3].name,
                                  reg[reg2].name, op1->value);
                       else if (op2->kind == CONSTANT_operand)
                           printf("  addi %s, %s, %d\n", reg[reg3].name,
                                  reg[reg1].name, op2->value);
                       // 无常数
                       else*/
                // 别用这些，已经把常数存进寄存器了……找罪受
                printf("  add %s, %s, %s\n", reg[reg3].name,
                       reg[reg1].name, reg[reg2].name);
            }
            else if (p->kind == MINUS_in)
            {
                // 有常数
                /*        if (op1->kind == CONSTANT_operand)
                            printf("  addi %s, %s, %d\n", reg[reg3].name,
                                   reg[reg2].name, -op1->value);
                        else if (op2->kind == CONSTANT_operand)
                            printf("  addi %s, %s, %d\n", reg[reg3].name,
                                   reg[reg1].name, -op2->value);
                        // 无常数
                        else*/
                printf("  sub %s, %s, %s\n", reg[reg3].name,
                       reg[reg1].name, reg[reg2].name);
            }
            else if (p->kind == STAR_in)
            {
                printf("  mul %s, %s, %s\n", reg[reg3].name,
                       reg[reg1].name, reg[reg2].name);
            }
            else if (p->kind == DIV_in)
            {
                printf("  div %s, %s\n", reg[reg1].name, reg[reg2].name);
                printf("  mflo %s\n", reg[reg3].name);
            }
        }
        else if (p->kind == IF_in)
        {
            int reg1 = get_reg(p->recop.op1);
            int reg2 = get_reg(p->recop.op2);
            save_all_reg(); // 一个基本块的结束

            if (strcmp(p->recop.relop->name, "==") == 0)
                printf("  beq ");
            else if (strcmp(p->recop.relop->name, "!=") == 0)
                printf("  bne ");
            else if (strcmp(p->recop.relop->name, ">") == 0)
                printf("  bgt ");
            else if (strcmp(p->recop.relop->name, "<") == 0)
                printf("  blt ");
            else if (strcmp(p->recop.relop->name, ">=") == 0)
                printf("  bge ");
            else if (strcmp(p->recop.relop->name, "<=") == 0)
                printf("  ble ");
            printf("%s, %s, LABEL%d\n", reg[reg1].name,
                   reg[reg2].name, p->recop.op3->value);
        }
        p = p->next;
    }
}

void create_code()
{
    printf(".data\n");
    printf("_prompt: .asciiz \"Enter an integer:  \"\n");
    printf("_ret: .asciiz \"\\n\"\n");
    printf(".globl main\n"); // 主函数从这里开始
    printf(".text\n\n");

    init_reg();

    // 添加read
    printf("\nread:\n");
    into_func();
    printf("  li $v0, 4\n");
    printf("  la $a0, _prompt\n");
    printf("  syscall\n"); // 调用print_string打印提示
    printf("  li $v0, 5\n");
    printf("  syscall\n");
    leave_func();

    // 添加write
    printf("\nwrite:\n");
    into_func();
    printf("  li $v0, 1\n");
    printf("  syscall\n"); // 输出
    printf("  li $v0, 4\n");
    printf("  la $a0, _ret\n");
    printf("  syscall\n"); // 打一个换行符
    printf("  move $v0, $0\n");
    leave_func();
    // 样例有

    object_read(); // 逐句翻译
}