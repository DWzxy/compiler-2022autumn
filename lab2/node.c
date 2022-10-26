#include "node.h"

Node *semantic_tree; //语法分析树

Node *new_leaf(int line_num, char *name, char *text)
{
    Node *now = (Node *)malloc(sizeof(Node));
    now->line_num = line_num;
    now->child_num = 0;
    strcpy(now->name, name);
    //   printf("line %d:leaf %s\n",line_num,now->name);
    now->if_token = true;
    if (strcmp(name, "INT") == 0)
    {
        now->val.int_val = strtol(text, NULL, 0);
    }
    else if (strcmp(name, "FLOAT") == 0)
    {
        sscanf(text, "%lf", &now->val.float_val);
    }
    else if (strcmp(name, "ID") == 0)
    {
        strcpy(now->val.char_val, text);
    }
    else
    {
        strcpy(now->val.char_val, text);
    }
    now->child = NULL;
    now->next = NULL;
    return now;
}

Node *new_node(int line_num, char *name, int para_num, ...)
{
    Node *now = (Node *)malloc(sizeof(Node));
    now->line_num = line_num;
    now->child_num = para_num;
    strcpy(now->name, name);
    //  printf("line %d: %s\n",line_num,now->name);
    now->if_token = false;
    now->child = NULL;
    now->next = NULL;

    va_list args;
    va_start(args, para_num);
    Node *tmp = NULL;

    for (int i = 1; i <= para_num; i++)
    {
        tmp = va_arg(args, Node *);
        if (now->child == NULL)
        {
            now->child = tmp;
        }
        else
        {
            Node *i = now->child;
            while (i->next)
                i = i->next;
            i->next = tmp;
        }
    }
    return now;
}

void print_tree(Node *now, int depth)
{
    if (now == NULL)
        return;
    for (int i = 1; i <= depth; i++)
        printf("  ");
    printf("%s", now->name);
    if (strcmp(now->name, "INT") == 0)
    {
        printf(": %d", now->val.int_val);
    }
    else if (strcmp(now->name, "FLOAT") == 0)
    {
        printf(": %lf", now->val.float_val);
    }
    else if (strcmp(now->name, "ID") == 0)
    {
        printf(": %s", now->val.char_val);
    }
    else if (strcmp(now->name, "TYPE") == 0)
    {
        printf(": %s", now->val.char_val);
    }
    else
    {
        printf(" ");
    }
    if (!now->if_token)
        printf("(%d)", now->line_num);
    printf("\n");
    for (Node *i = now->child; i; i = i->next)
    {
        print_tree(i, depth + 1);
    }
}