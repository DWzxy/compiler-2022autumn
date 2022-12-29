#include "optimize.h"
FILE *p;

int main(int argc, char **argv)
{
    if (argc > 2)
        freopen(argv[2], "w", stdout);

    if (argc > 1)
        p = fopen(argv[1], "r");

    optimize_read();
  //  clear();
  //  clear1();
  //  clear2();

    deal();

    // printf("print all \n");
    print_all_intercode();
    return 0;
}

/*
20
2 0 8 5 13 17 19 14 11 6 1 9 10 15 3 18 7 12 16 4
*/