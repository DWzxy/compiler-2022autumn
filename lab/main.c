#include "syntax.tab.h"
#include "semantic.h"
#include "avl.h"
#include "intermediate.h"

extern int yyparse();
extern void yyrestart(FILE *);
extern Node *semantic_tree;
extern int if_over;        //语法分析是否正确
extern int semantic_right; //符号分析是否正确

int main(int argc, char **argv)
{
    //词法语法分析
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
    if (!if_over)
        return 0;

    //符号错误检查
    table_init();
    read(semantic_tree); 
    check_error18();
    printf("[[[semantic over.]]]\n\n\n");

//中间代码生成
    if (semantic_right)
    {
        trans_read(semantic_tree);
        printf("[[[interdiate over.]]]\n\n\n");
 //       print_all_intercode();
    }

    return 0;
}

/*
20
2 0 8 5 13 17 19 14 11 6 1 9 10 15 3 18 7 12 16 4
*/