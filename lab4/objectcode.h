#pragma once
#include"intermediate.h"

struct var_descriper;
typedef struct Operand_ Operand;

typedef struct reg_descriper
{
    char name[6];
    struct Operand_ *var_store;
    int age;     //已经保存的时间
} Reg_descriper; //寄存器，保存里面存储的变量

/*
typedef struct var_descriper
{
    char name[6];
    struct reg_descriper *Reg_store;
    struct var_descriper *next;
} Var_descriper; //变量，保存值所在的寄存器（可能不止一个）
*/

void init_reg();

//将x加入参数与变量列表中
void add_op(Operand*x);
//计算该函数的参数变量空间,注意不是总数
int para_var_count(InterCode*x);
//查找offset,将*x与x联系起来
int find_op_offset(Operand*x);
//改变offset,加入列表,注意没有真正调用push
void push_op(Operand*x);
//返回寄存器
int get_reg(Operand *x);
void save_all_reg();
void into_func();
void leave_func();

bool op_equal(Operand *x, Operand *y);
void object_read();
void create_code();