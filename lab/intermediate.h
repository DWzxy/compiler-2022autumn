#pragma once
#include "node.h"

struct Type_;
struct ListNode_;
typedef struct Type_ Type;
typedef struct ListNode_ ListNode;

enum Operand_kind
{
    VARIABLE_operand,
    CONSTANT_operand,
    TMP_operand,
    PARA_operand,
    LABEL_operand,
    FUNCTION_operand
};

enum Intercode_kind
{
    // singop
    LABEL_in,
    FUNCTION_in,
    GOTO_in,
    RETURN_in,
    ARG_in,
    PARAM_in,
    READ_in,
    WRITE_in,
    // binop
    ASSIGN_in,
    DEC_in,
    CALL_in,
    // triop
    PLUS_in,
    MINUS_in,
    STAR_in,
    DIV_in,
    // recop
    IF_in
};

typedef struct Operand_
{
    enum Operand_kind kind; //不同的类型，如立即数，变量，函数
    enum Operand_type
    {
        Normal,
        Address,
        Star
    } type; //是普通操作数还是取地址，取值
    char name[30];
    int value;
} Operand;

typedef struct InterCode_
{
    enum Intercode_kind kind;
    union
    {
        struct
        {
            Operand *op;
        } singop; // LABEL,FUNCTION,GOTO,RETURN,ARG,PARAM,READ,WRITE
        struct
        {
            Operand *op1, *op2;
        } binop; // ASSIGN,DEC,CALL
        struct
        {
            Operand *result, *op1, *op2;
        } triop; // ADD,SUB,MUL,DIV
        struct
        {
            Operand *relop, *op1, *op2, *op3;
        } recop; // IF
    };
    struct InterCode_ *next;
    struct InterCode_ *pre;
} InterCode;

void trans_init();
void insert_intercode(InterCode *k);
Operand *new_operand(enum Operand_kind kind, int type, char *name);
void new_intercode(enum Intercode_kind kind, Operand *res,
                   Operand *op1, Operand *op2, Operand *op3);
Operand *new_label();
Operand *new_tmp();
Operand *new_para();
Operand *new_constant();
Operand *new_varible(char *name);
Operand *new_function(char *name);
void print_operand(Operand *k);
void print_intercode(InterCode *k);
void print_all_intercode();
int size_of(Type *k);

void trans_ExtDef(Node *k);
void trans_FunDec(Node *k);
void trans_VarList(Node *k);
void trans_DefList(Node *k);
void trans_StmtList(Node *k);
void trans_CompSt(Node *k);
void trans_ExtDecList(Node *k);
void trans_Dec(Node *k);
ListNode *trans_VarDec(Node *k);
void trans_stmt(Node *k);
Operand *trans_exp(Node *k);
Operand *trans_arg(Node *k);
void trans_cond(Node *k, Operand *true_label, Operand *false_label);

void trans_read(Node *k);

extern bool type_equal(Type *type_a, Type *type_b);
extern bool if_left(Node *k);
extern void error(int k, int line, char *s, char *y);