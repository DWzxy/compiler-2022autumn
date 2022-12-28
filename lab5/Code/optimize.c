#include "optimize.h"

// #define DEBUG

extern FILE *p;
extern InterCode *intercode_head;

Block *block_list = NULL; // 所有的基本块
Block *block_tail = NULL;
BlockList *entry_list = NULL; // 所有的入口
int block_id = 0;

void block_init()
{
}

Block *new_block(InterCode *head)
{
    Block *now = (Block *)malloc(sizeof(Block));
    now->head = head;
    now->tail = NULL;
    now->next = NULL;
    now->edge = NULL;
    now->id = block_id++;

    // 加入集合
    if (block_list == NULL)
        block_list = block_tail = now;
    else
    {
        block_tail->next = now;
        block_tail = now;
    }
    return now;
}

void new_edge(Block *x, Block *y)
{
    Edge *now = (Edge *)malloc(sizeof(Edge));
    now->from = x;
    now->to = y;
    now->next = x->edge;
    x->edge = now;
}

void build_block()
{
    bool flag;
    Block *tmp = new_block(intercode_head);
    for (InterCode *i = intercode_head; i; i = i->next)
    {
        flag = false;
        if (i->kind == GOTO_in || i->kind == IF_in ||
            i->kind == RETURN_in)
        {
            flag = true;
            if (i->kind == IF_in && i->next != NULL &&
                i->next->kind == GOTO_in)
                flag = false;
        }

        if (i->next != NULL && (i->next->kind == FUNCTION_in ||
                                i->next->kind == LABEL_in))
            flag = true;
        if (i->next == NULL)
            flag = true;

        if (i->kind == FUNCTION_in)
        {
            BlockList *now = (BlockList *)malloc(sizeof(BlockList));
            now->next = entry_list;
            now->block = tmp;
            entry_list = now;
        }

        if (!flag)
            continue;

        tmp->tail = i;
        if (i->next)
            tmp = new_block(i->next);
    }
}

void build_graph()
{
#ifdef DEBUG
    printf("[entry] :");
    for (BlockList *i = entry_list; i; i = i->next)
    {
        printf("%d ", i->block->id);
    }
    printf("\n");
#endif

    Block *tmp = block_list;
    for (InterCode *i = intercode_head; i; i = i->next)
    {
        if (i->kind == GOTO_in)
        {
            Block *to = find_block(i->singop.op->name);
            new_edge(tmp, to);
        }
        else if (i->kind == IF_in)
        {
            Block *to = find_block(i->recop.op3->name);
            new_edge(tmp, to);
        }
        if (i == tmp->tail)
        {
            if (i->kind != RETURN_in &&
                i->kind != GOTO_in && i->kind != IF_in)
            {
                if (tmp->next != NULL)
                    new_edge(tmp, tmp->next);
            }
            tmp = tmp->next;
        }
    }
}

Block *find_block(char *name)
{
    for (Block *i = block_list; i; i = i->next)
    {
        if (i->head->kind != LABEL_in)
            continue;
        if (Cmp(i->head->singop.op->name, name))
            return i;
    }
    return NULL;
}

void print_block()
{
    for (Block *i = block_list; i; i = i->next)
    {
        printf("----------- block%d --------\n", i->id);
        for (InterCode *j = i->head;; j = j->next)
        {
            print_intercode(j);
            if (j == i->tail)
                break;
        }
        printf("\n");
    }
}

void print_graph()
{
    for (Block *i = block_list; i; i = i->next)
    {
        printf(" block%d\n  -> ", i->id);
        for (Edge *j = i->edge; j; j = j->next)
        {
            printf("%d ", j->to->id);
        }
        printf("\n");
    }
}

void optimize_read()
{
    char c;
    char s[101];
    Operand *t;
    while (1)
    {
        if (!try_end())
            break;
        t = new_variable();
        if (Cmp(t->name, "LABEL"))
        {
            Operand *now = new_variable();
            read_str(s);
            new_intercode(LABEL_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(t->name, "FUNCTION"))
        {
            Operand *now = new_variable();
            read_str(s);
            new_intercode(FUNCTION_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(t->name, "GOTO"))
        {
            Operand *now = new_variable();
            new_intercode(GOTO_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(t->name, "RETURN"))
        {
            Operand *now = new_variable();
            new_intercode(RETURN_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(t->name, "ARG"))
        {
            Operand *now = new_variable();
            new_intercode(ARG_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(t->name, "PARAM"))
        {
            Operand *now = new_variable();
            new_intercode(PARAM_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(t->name, "READ"))
        {
            Operand *now = new_variable();
            new_intercode(READ_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(t->name, "WRITE"))
        {
            Operand *now = new_variable();
            new_intercode(WRITE_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(t->name, "DEC"))
        {
            Operand *now = new_variable();
            int size;
            fscanf(p, "%d", &size);
            new_intercode(DEC_in, NULL, now, new_constant(size), NULL);
        }
        else if (Cmp(t->name, "IF"))
        {
            Operand *op1 = new_variable();
            Operand *relop = new_variable();
            Operand *op2 = new_variable();
            fscanf(p, "%s", s);
            Operand *op3 = new_variable();
            new_intercode(IF_in, relop, op1, op2, op3);
        }
        else
        {
            //:=
            fscanf(p, "%s", s);
            Operand *y = new_variable();
            if (Cmp(y->name, "CALL"))
            {
                Operand *z = new_variable();
                new_intercode(CALL_in, NULL, t, z, NULL);
                continue;
            }

            // 下面看有没有z
            bool flag = false;
            char c;
            while (1)
            {
                fscanf(p, "%c", &c);
                if (c == ' ')
                    continue;
                if (c == '\n')
                    break;
                flag = true;
                break;
            }
            if (!flag)
            {
                new_intercode(ASSIGN_in, NULL, t, y, NULL);
                continue;
            }

            Operand *z = new_variable();
            if (c == '+')
                new_intercode(PLUS_in, t, y, z, NULL);
            else if (c == '-')
                new_intercode(MINUS_in, t, y, z, NULL);
            else if (c == '*')
                new_intercode(STAR_in, t, y, z, NULL);
            else if (c == '/')
                new_intercode(DIV_in, t, y, z, NULL);
        }
    }
}

Operand *new_variable()
{
    char t[101];
    char c;
    Operand *now = new_function("");

    while (fscanf(p, "%c", &c))
    {
        if (c != ' ')
            break;
    }

    if (c == '#')
    {
        now->kind = CONSTANT_operand;
        int tmp;
        fscanf(p, "%d", &tmp);
        now->value = tmp;
    }
    else
    {
        if (c == '*')
            now->type = Star;
        else if (c == '&')
            now->type = Address;
        else
            fseek(p, -1, SEEK_CUR);

        read_str(t);
        strcpy(now->name, t);
    }
    return now;
}

void read_str(char *s)
{
    fscanf(p, "%s", s);
}

bool Cmp(char *a, char *b)
{
    return strcmp(a, b) == 0;
}

bool try_end()
{
    char c;
    fscanf(p, "%c", &c);
    while (1)
    {
        if (feof(p))
            return false;

        if (c != ' ' && c != '\n')
        {
            fseek(p, -1, SEEK_CUR);
            break;
        }
        fscanf(p, "%c", &c);
    }

    return true;
}

void common_expression()
{
         DAG_pointer *dag_pointer_head = NULL;
    for (Block *i = block_list; i; i = i->next)
    {
// 一个新的DAG
#ifdef DEBUG
        printf("---------- block %d ---------\n", i->id);
#endif
   
        for (InterCode *k = i->head;; k = k->next)
        {
            if (k->para_num != 3 && k->kind != ASSIGN_in)
            { // 只替换
                if (k->kind == ARG_in)
                {
                    DAG_pointer *x = find_DAG_pointer(
                        &dag_pointer_head, k->singop.op);
                    k->singop.op = x->node->pointer->variable;
                }
                else if (k->kind == RETURN_in)
                {
                    DAG_pointer *x = find_DAG_pointer(
                        &dag_pointer_head, k->singop.op);
                    k->singop.op = x->node->pointer->variable;
                }
                else if (k->kind == WRITE_in)
                {
                    DAG_pointer *x = find_DAG_pointer(
                        &dag_pointer_head, k->singop.op);
                    k->singop.op = x->node->pointer->variable;
                }
                else if (k->kind == IF_in)
                {
                    DAG_pointer *x = find_DAG_pointer(
                        &dag_pointer_head, k->recop.op1);
                    DAG_pointer *y = find_DAG_pointer(
                        &dag_pointer_head, k->recop.op2);
                    k->recop.op1 = x->node->pointer->variable;
                    k->recop.op2 = y->node->pointer->variable;
                }
                if (k == i->tail)
                    break;
                continue;
            }

#ifdef DEBUG
            printf("    now trans: ");
            print_intercode(k);
#endif
            if (k->kind == ASSIGN_in)
            {
                DAG_pointer *x = find_DAG_pointer(
                    &dag_pointer_head, k->binop.op1);
                DAG_pointer *y = find_DAG_pointer(
                    &dag_pointer_head, k->binop.op2);
                k->binop.op2 = y->node->pointer->variable;

                // 找替代
                bool flag = false;
                for (DAG_pointer *now = dag_pointer_head; now; now = now->next)
                {
                    if (strcmp(now->node->op, "=") != 0)
                        continue;
                    if (now->node->lc == y->node)
                    {
#ifdef DEBUG
                        printf("find replace ");
                        print_operand(now->node->pointer->variable);
                        printf(" = ");
                        print_operand(now->node->lc->pointer->variable);
                        printf("\n");

                        print_operand(x->variable);
                        printf(" -> ");
                        print_operand(now->node->pointer->variable);
                        printf("\n");
#endif
                        flag = true;

                        if (x->node->pointer == x)
                            x->node->pointer = NULL;
                        x->node = now->node;
                        remove_intercode(k);
                        break;
                    }
                }
                if (flag == false)
                {
                    DAG_node *now = (DAG_node *)malloc(sizeof(DAG_node));
                    now->lc = y->node;
                    now->rc = NULL;
                    strcpy(now->op, "=");
                    if (x->node->pointer == x)
                        x->node->pointer = NULL;
                    x->node = now;
                    now->pointer = x;
#ifdef DEBUG
                    print_operand(x->variable);
                    printf(" = ");
                    print_operand(y->variable);
                    printf("\n");
#endif
                }
            }
            else
            {
                DAG_pointer *x = find_DAG_pointer(
                    &dag_pointer_head, k->triop.result);
                DAG_pointer *y = find_DAG_pointer(
                    &dag_pointer_head, k->triop.op1);
                DAG_pointer *z = find_DAG_pointer(
                    &dag_pointer_head, k->triop.op2);
                k->triop.op1 = y->node->pointer->variable;
                k->triop.op2 = z->node->pointer->variable;

                // 找替代
                bool flag = false;
                for (DAG_pointer *now = dag_pointer_head; now; now = now->next)
                {
                    if (k->kind == PLUS_in &&
                        strcmp(now->node->op, "+") != 0)
                        continue;
                    if (k->kind == MINUS_in &&
                        strcmp(now->node->op, "-") != 0)
                        continue;
                    if (k->kind == STAR_in &&
                        strcmp(now->node->op, "*") != 0)
                        continue;
                    if (k->kind == DIV_in &&
                        strcmp(now->node->op, "/") != 0)
                        continue;
                    if (now->node->lc == y->node &&
                        now->node->rc == z->node)
                    {
#ifdef DEBUG
                        printf("find replace ");
                        print_operand(now->node->pointer->variable);
                        printf(" = ");
                        print_operand(now->node->lc->pointer->variable);
                        printf(" $ ");
                        print_operand(now->node->rc->pointer->variable);
                        printf("\n");

                        print_operand(x->variable);
                        printf(" -> ");
                        print_operand(now->node->pointer->variable);
                        printf("\n");
#endif
                        flag = true;
                        if (x->node->pointer == x)
                            x->node->pointer = NULL;
                        x->node = now->node;
                        remove_intercode(k);
                        break;
                    }
                }
                if (flag == false)
                {
                    DAG_node *now = (DAG_node *)malloc(sizeof(DAG_node));
                    now->lc = y->node;
                    now->rc = z->node;
                    if (x->node->pointer == x)
                        x->node->pointer = NULL;
                    x->node = now;
                    now->pointer = x;

                    if (k->kind == PLUS_in)
                        strcpy(now->op, "+");
                    else if (k->kind == MINUS_in)
                        strcpy(now->op, "-");
                    else if (k->kind == STAR_in)
                        strcpy(now->op, "*");
                    else if (k->kind == DIV_in)
                        strcpy(now->op, "/");
                }
            }
#ifdef DEBUG
            printf("    trans over: ");
            print_intercode(k);
#endif
            if (k == i->tail)
                break;
        }
    }
}

DAG_pointer *find_DAG_pointer(DAG_pointer **head, Operand *x)
{
    for (DAG_pointer *i = *head; i; i = i->next)
    {
        if (operand_equal(i->variable, x))
            return i;
    }
#ifdef DEBUG
    printf("create new %s %d\n", x->name, x->value);
#endif

    DAG_node *tmp = (DAG_node *)malloc(sizeof(DAG_node));
    tmp->lc = NULL;
    tmp->rc = NULL;
    tmp->op[0] = 0;

    DAG_pointer *now = (DAG_pointer *)malloc(sizeof(DAG_pointer));
    now->node = tmp;

    tmp->pointer = now;
    now->variable = x;
    now->next = *head;
    *head = now;
    return now;
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
        return Cmp(x->name, y->name);
}