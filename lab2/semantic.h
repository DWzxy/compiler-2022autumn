#pragma once
#include "node.h"

typedef struct avl_node AVL_node;

enum Kind
{
    BASIC,
    ARRAY,
    STRUCTURE,
    FUNC
};

enum BasicKind
{
    Basic_int,
    Basic_float
};

typedef struct Type_
{
    enum Kind kind;
    union
    { // 基本类型
        enum BasicKind basic;

        // 数组类型信息：包括元素类型与数组大小构成
        struct
        {
            struct Type_ *elem;
            int size;
        } array;

        // 结构体类型信息：是一个链表
        struct ListNode_ *structure;

        //函数类型信息
        struct
        {
            int para_num;
            struct Type_ *func_type;
            struct ListNode_ *para;
        } func;
    };
} Type;

typedef struct ListNode_
{
    char name[30];          // 域的名字
    struct Type_ *type;     // 域的类型
    struct ListNode_ *next; // 下一个域
} ListNode;

Type *new_type(enum Kind kind);
Type *copy_type(Type *x);
ListNode *copy_listnode(ListNode *x);
ListNode *create_new_listnode();
ListNode *new_listnode(char *name, Type *type);

void deal_ExtDef(Node *k);
ListNode *deal_FunDec(Node *k, Type *type);
Type *deal_Specifier(Node *k);
Type *deal_StructSpecifier(Node *k);
ListNode *deal_DefList(Node *k);
void deal_StmtList(Node *k, Type *t);
ListNode *deal_CompSt(Node *k, Type *t);
ListNode *deal_VarList(Node *k);
void deal_ExtDecList(Node *k, Type *type);
ListNode *deal_Dec(Node *k, Type *type);
ListNode *deal_VarDec(Node *k, Type *type);

void read(Node *k);
void insert_listnode(ListNode *k, AVL_node **table);
void check_stmt(Node *k, Type *t);
Type *check_exp(Node *k);
bool type_equal(Type *type_a, Type *type_b);
bool if_left(Node *k);

bool check_error1(char *name, int line_num);
bool check_error2(char *name, int line_num);
bool check_error3(char *name, int line_num);
bool check_error4(char *name, int line_num);
bool check_error5(Type *a, Type *b, int line_num);
bool check_error6(Node *k, int line_num);
bool check_error7(Type *a, Type *b, int line_num);
bool check_error8(Type *a, Type *b, int line_num);
bool check_error9(ListNode *a, Type *b, int line_num);
bool check_error10(Type*a, int line_num);
void error(int k, int line, char *s, char *y);
