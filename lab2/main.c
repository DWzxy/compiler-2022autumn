#include "syntax.tab.h"
#include "semantic.h"
#include "avl.h"

extern int yyparse();
extern void yyrestart(FILE *);
extern Node *semantic_tree;
extern AVL_node *symbol_define_table;
extern AVL_node *symbol_announce_table; //符号表

int main(int argc, char **argv)
{
    if (argc > 1)
    {

        FILE *f = fopen(argv[1], "r");
        if (!f)
        {
            perror(argv[1]);
            return 1;
        }
        yyrestart(f);
        yyparse();
    }

    read(semantic_tree);
  /*  printf("\n\n\n\n\n");
    printf("[[[[announce_table]]]]\n");
    print_avl_tree(symbol_announce_table);
    printf("\n\n\n\n\n[[[[define_table]]]]\n");
    print_avl_tree(symbol_define_table);
    printf("\n");
*/

    return 0;
}

/*
20
2 0 8 5 13 17 19 14 11 6 1 9 10 15 3 18 7 12 16 4
*/