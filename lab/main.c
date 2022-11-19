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
    if (argc > 2)
        freopen(argv[2], "w", stdout);
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

    //中间代码生成
    if (semantic_right)
    {
        trans_init();
        trans_read(semantic_tree);
        clear();
        clear2();
        print_all_intercode();
    }
    return 0;
}

/*
clear   goto     数字运算   return-(label||function)
clear2  label
*/