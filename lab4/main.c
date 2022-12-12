#include "syntax.tab.h"
#include "semantic.h"
#include "avl.h"
#include "intermediate.h"
#include "objectcode.h"

extern int yyparse();
extern void yyrestart(FILE *);
extern Node *semantic_tree;
extern int if_over;        // 语法分析是否正确
extern int semantic_right; // 符号分析是否正确

int main(int argc, char **argv)
{
    // 词法语法分析
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

    // 符号错误检查
    if (!if_over)
        return 0;
    table_init();
    read(semantic_tree);
    check_error18();

    // 中间代码生成
    if (!semantic_right)
        return 0;
    trans_init();
    trans_read(semantic_tree);
    //  clear();
    //  clear1();
    //   clear2();
    print_all_intercode();
    printf("\n\n");

    // 机器代码生成
    if (argc > 2)
        freopen(argv[2], "w", stdout);
    create_code();
    return 0;
}

/*
20
2 0 8 5 13 17 19 14 11 6 1 9 10 15 3 18 7 12 16 4
*/