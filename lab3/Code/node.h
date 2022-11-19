#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

struct Type_;
struct ListNode_;
struct avl_node;
typedef struct Type_ Type;
typedef struct ListNode_ ListNode;
typedef struct avl_node AVL_node;

typedef union
{
    int int_val;
    double float_val;
    char char_val[108];
} Valtype; //节点的值，分别对应int,float,id，其它类型的值(yytext)也都存在char_val中

typedef struct node Node;
struct node
{
    int line_num;
    int child_num;
    char name[30];      //名字，即语法分析时获得的类型名称
    Valtype val;        //值
    int if_token;       //用于记录是否是词法节点
    struct node *child; //第一个孩子节点
    struct node *next;  //下一个兄弟节点
    AVL_node *symbol;   //该节点下的符号表
    //理论上只有Compst和program才有
};

#define YYSTYPE Node *
Node *new_leaf(int line_num, char *name, char *text);
Node *new_node(int line_num, char *name, int para_num, ...);
void print_tree(Node *k, int depth);