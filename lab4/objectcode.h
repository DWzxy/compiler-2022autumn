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

void add_op(Operand*x);
int find_op_offset(Operand*x);
void push_op(Operand*x);
int get_reg(Operand *x);
void save_all_reg();

bool op_equal(Operand *x, Operand *y);
void object_read();
void create_code();