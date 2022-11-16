#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct ListNode_ ListNode;
typedef struct Type_ Type;
enum Kind;
enum Table_kind
{
	announce,
	define
};

typedef struct avl_node
{
	ListNode *node;//记录了这个节点代表的符号
	//	int num;
	int cnt; //相同人数的节点个数
	struct avl_node *lc;
	struct avl_node *rc;
	int bf;	 //平衡因子
	int sum; //该树总节点个数（不包括自己）
} AVL_node;

void table_init();
void stack_push();
void stack_pop();
void stack_change(AVL_node*k);

// AVL_node* new_avl_node(int num);//创建新节点
int cmp(AVL_node *a, AVL_node *b);
AVL_node *new_avl_node(ListNode *k); //创建新节点
void RotateL(AVL_node **ptr);
void RotateR(AVL_node **ptr);
void RotateLR(AVL_node **ptr);
void RotateRL(AVL_node **ptr);
// bool Insert(AVL_node **ptr, int num);
bool Insert(AVL_node **ptr, ListNode *k); //添加
AVL_node *search(int x, AVL_node *k);

void print_avl_tree(AVL_node *k);
void print_avl_listnode(ListNode *k);
void print_avl_type(Type *k);

ListNode *avl_search_listnode(AVL_node *k, char *name, enum Kind kind);
Type *avl_search_type(AVL_node *k, char *name);
ListNode *search_listnode(enum Table_kind m, char *name,
						  enum Kind kind, bool flag);
ListNode *search_all_listnode(enum Table_kind m, char *name, bool flag);
Type *search_type(enum Table_kind m, char *name, bool flag);
void check_announce(AVL_node *k, enum Kind kind);