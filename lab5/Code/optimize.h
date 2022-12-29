#include "intermediate.h"

struct Edge_;

typedef struct Block_
{
    int id;
    InterCode *head;
    InterCode *tail;
    struct Edge_ *edge;
    struct Block_ *next;
    struct Block_ *pre;
} Block;

typedef struct Edge_
{
    Block *from;
    Block *to;
    struct Edge_ *next;
} Edge;

void deal();
void block_init();
Block *new_block(InterCode *head);
void new_edge(Block *x, Block *y);
void build_block(InterCode *x);
void build_graph();
// 找label
Block *find_block(char *name);
void print_block();
void print_graph();

void optimize_read();

Operand *new_variable();
Operand *add_variable(Operand *x);
void read_str(char *s);
bool Cmp(char *a, char *b);
bool try_end();

struct DAG_pointer_;

typedef struct DAG_node_
{
    char op[5];
    struct DAG_node_ *lc;
    struct DAG_node_ *rc;
    struct DAG_node_ *parent;
    struct DAG_node_ *next;
    struct DAG_pointer_ *pointer; // 互指
    InterCode *intercode;
} DAG_node;

typedef struct DAG_pointer_
{
    Operand *variable;
    DAG_node *node; // 互指
    struct DAG_pointer_ *next;
} DAG_pointer;

void common_expression();
DAG_pointer *find_DAG_pointer(DAG_pointer **head, Operand *x);
DAG_node *new_dag_node(char *name, DAG_node *lc,
                       DAG_node *rc, InterCode *k, DAG_pointer *x);

void check_dead_code(DAG_node *x, Block *block);
void dead_code();
bool operand_equal_minus(Operand *x, Operand *y);

void live_variable();
void live_init();
void live_leave();