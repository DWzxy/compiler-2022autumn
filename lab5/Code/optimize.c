#include "optimize.h"

#define DEBUG

extern FILE *p;
extern InterCode *intercode_head;

Block *block_list = NULL; // 所有的基本块
Block *block_tail = NULL;
int block_id = 1;
int variable_count = 0; // 变量数目
DAG_pointer *dag_pointer_head = NULL;
Operand *operand_list = NULL; // 变量列表
bool **in;
bool **out;

void deal()
{
    for (InterCode *i = intercode_head; i; i = i->next)
    {
        if (i->kind != FUNCTION_in)
            continue;
        // 以单个函数为单位

        block_init();
        build_block(i);
        print_block();
        build_graph();
        print_graph();
        live_init();
        live_variable();
        live_leave();

        //      common_expression();
    }
}

void block_init()
{
    block_list = NULL; // 所有的基本块
    block_tail = NULL;
    block_id = 1;
    dag_pointer_head = NULL;
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

void build_block(InterCode *x)
{
    bool flag;
    Block *tmp = new_block(x);
    for (InterCode *i = x->next; i; i = i->next)
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
        if (i->next != NULL && i->next->kind == LABEL_in)
            flag = true;
        if (i->next != NULL && i->next->kind == FUNCTION_in)
            flag = true;

        tmp->tail = i;
        if (flag)
        {
            if (i->next != NULL && i->next->kind != FUNCTION_in)
                tmp = new_block(i->next);
            else
                break;
        }
    }
#ifdef DEBUG
    printf("variable_count = %d\n", variable_count);
#endif
}

void build_graph()
{
    Block *exit = (Block *)malloc(sizeof(Block));
    exit->head = NULL;
    exit->tail = NULL;
    exit->next = NULL;
    exit->edge = NULL;
    exit->id = -1;

    for (Block *k = block_list; k; k = k->next)
    {
        for (InterCode *i = k->head; i; i = i->next)
        {
            if (i->kind == GOTO_in)
            {
                Block *to = find_block(i->singop.op->name);
                new_edge(k, to);
            }
            else if (i->kind == IF_in)
            {
                Block *to = find_block(i->recop.op3->name);
                new_edge(k, to);
            }
            else if (i->kind == RETURN_in)
            {
                new_edge(k, exit);
            }
            if (i == k->tail)
            {
                if (i->kind != RETURN_in &&
                    i->kind != GOTO_in)
                {
                    if (k->next != NULL)
                        new_edge(k, k->next);
                }
                break;
            }
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
    while (1)
    {
        if (!try_end())
            break;
        read_str(s);
        if (Cmp(s, "LABEL"))
        {
            read_str(s);
            new_intercode(LABEL_in, NULL, new_function(s), NULL, NULL);
            read_str(s);
        }
        else if (Cmp(s, "FUNCTION"))
        {
            read_str(s);
            new_intercode(FUNCTION_in, NULL, new_function(s), NULL, NULL);
            read_str(s);
        }
        else if (Cmp(s, "GOTO"))
        {
            read_str(s);
            new_intercode(GOTO_in, NULL, new_function(s), NULL, NULL);
        }
        else if (Cmp(s, "RETURN"))
        {
            Operand *now = new_variable();
            new_intercode(RETURN_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(s, "ARG"))
        {
            Operand *now = new_variable();
            new_intercode(ARG_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(s, "PARAM"))
        {
            Operand *now = new_variable();
            new_intercode(PARAM_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(s, "READ"))
        {
            Operand *now = new_variable();
            new_intercode(READ_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(s, "WRITE"))
        {
            Operand *now = new_variable();
            new_intercode(WRITE_in, NULL, now, NULL, NULL);
        }
        else if (Cmp(s, "DEC"))
        {
            Operand *now = new_variable();
            int size;
            fscanf(p, "%d", &size);
            new_intercode(DEC_in, NULL, now, new_constant(size), NULL);
        }
        else if (Cmp(s, "IF"))
        {
            char relop[101];
            Operand *op1 = new_variable();
            read_str(relop);
            Operand *op2 = new_variable();
            read_str(s);
            read_str(s);
            new_intercode(IF_in, new_function(relop), op1, op2, new_function(s));
        }
        else
        {

            Operand *x = new_function(s);
            add_variable(x);
            read_str(s); //:=
            read_str(s);
            if (Cmp(s, "CALL"))
            {
                Operand *z = new_variable();
                new_intercode(CALL_in, NULL, x, z, NULL);
                continue;
            }

            Operand *y = new_function(s);
            add_variable(y);
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
                new_intercode(ASSIGN_in, NULL, x, y, NULL);
                continue;
            }

            Operand *z = new_variable();
            if (c == '+')
                new_intercode(PLUS_in, x, y, z, NULL);
            else if (c == '-')
                new_intercode(MINUS_in, x, y, z, NULL);
            else if (c == '*')
                new_intercode(STAR_in, x, y, z, NULL);
            else if (c == '/')
                new_intercode(DIV_in, x, y, z, NULL);
        }
    }
}

Operand *new_variable()
{
    char t[101];
    char c;
    int tmp;
    Operand *now = new_function("");

    while (fscanf(p, "%c", &c))
    {
        if (c != ' ')
            break;
    }

    if (c == '#')
    {
        now->kind = CONSTANT_operand;
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
    now = add_variable(now);
    return now;
}

Operand *add_variable(Operand *now)
{
    for (Operand *i = operand_list; i; i = i->next)
    {
        if (operand_equal(i, now))
        {
            free(now);
            return i;
        }
    }

    // 新的变量
    variable_count++;
    now->offset = variable_count;
#ifdef DEBUG
    printf("new va %d: ", now->offset);
    print_operand(now);
    printf("\n");
#endif
    if (operand_list == NULL)
        operand_list = now;
    else
    {
        now->next = operand_list;
        operand_list = now;
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

    for (Block *i = block_list; i; i = i->next)
    {
// 一个新的DAG
#ifdef DEBUG
        printf("---------- block %d ---------\n", i->id);
#endif
        dag_pointer_head = NULL;
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

void live_init()
{
    int m = block_id + 1;
    int n = variable_count;
    in = (bool **)malloc(sizeof(bool *) * m);
    out = (bool **)malloc(sizeof(bool *) * m);
    for (int i = 0; i < m; i++)
    {
        out[i] = (bool *)malloc(sizeof(bool) * n);
        in[i] = (bool *)malloc(sizeof(bool) * n);
        memset(out[i], false, sizeof(bool) * n);
        memset(in[i], false, sizeof(bool) * n);
    }
}

void live_leave()
{
    int m = block_id + 1;
    for (int i = 0; i < m; i++)
    {
        free(out[i]);
        free(in[i]);
    }
    free(out);
    free(in);
}

void live_variable()
{
    live_init();
    // 块的数目为block_id+1
    // 变量数目为variable_count
    int m = block_id + 1;
    int n = variable_count;
    // 第一维是不同的块，第二维是每个变量
    bool if_changed = true;
    while (if_changed)
    {
        if_changed = false;

        for (Block *k = block_list; k; k = k->next)
        {
#ifdef DEBUG
            printf("-------- now refresh block %d --------\n", k->id);
            for (int j = 1; j <= variable_count; j++)
                printf("%d ", out[k->id][j]);
            printf("\n-> \n");
#endif
            // 更新OUT
            for (Edge *i = k->edge; i; i = i->next)
            {
                Block *to = i->to;
                if (to->id == -1)
                    continue;
                for (int j = 1; j <= variable_count; j++)
                    out[k->id][j] |= in[to->id][j];
                // OUT[B] = ∪ S是B的一个后继 IN[S];
            }
#ifdef DEBUG
            for (int j = 1; j <= variable_count; j++)
                printf("%d ", out[k->id][j]);
            printf("\n");
#endif

            // 更新IN
            bool *new_in = (bool *)malloc(sizeof(bool) * n);
            for (int j = 1; j <= n; j++)
                new_in[j] = out[k->id][j];
                          printf("free new_in\n");
            free(new_in);
            printf("free new_in\n");
            continue;
            for (InterCode *i = k->tail; i != k->head->pre; i = i->pre)
            {
                int def = 0;
                int use1 = 0, use2 = 0;
                if (i->kind == ARG_in || i->kind == WRITE_in || i->kind == RETURN_in)
                {
                    use1 = i->singop.op->offset;
                }
                else if (i->kind == READ_in)
                {
                    def = i->singop.op->offset;
                }
                else if (i->kind == CALL_in)
                {
                    if (i->binop.op1->type == Normal)
                        def = i->binop.op1->offset;
                }
                else if (i->kind == IF_in)
                {
                    Operand *x = i->recop.op1;
                    Operand *y = i->recop.op2;
                    use1 = x->offset;
                    use2 = y->offset;
                }
                else if (i->kind == ASSIGN_in)
                {
                    Operand *x = i->binop.op1;
                    Operand *y = i->binop.op2;
                    if (x->type == Normal)
                        def = x->offset;
                    use1 = y->offset;
                }
                else if (i->para_num == 3)
                {
                    Operand *x = i->triop.result;
                    Operand *y = i->triop.op1;
                    Operand *z = i->triop.op2;
                    if (x->type == Normal)
                        def = x->offset;
                    use1 = y->offset;
                    use2 = z->offset;
                }
                else
                    continue;

                new_in[def] = false;
                new_in[use1] = true;
                new_in[use2] = true;
#ifdef DEBUG
                print_intercode(i);
                printf("def %d  use1 %d  use2 %d\n", def, use1, use2);
#endif
            }
#ifdef DEBUG
            printf("IN: ");
            for (int j = 1; j <= n; j++)
                printf("%d ", new_in[j]);
            printf("\n");

            for (Operand *i = operand_list; i; i = i->next)
            {
                if (new_in[i->offset])
                    print_operand(i), printf(" ");
            }
            printf("\n");
#endif
            for (int j = 1; j <= n; j++)
            {
                if (new_in[j] != in[k->id][j])
                    if_changed = true;
                in[k->id][j] = new_in[j];
            }
            // free(new_in);
        }
    }
  //  live_leave();
}