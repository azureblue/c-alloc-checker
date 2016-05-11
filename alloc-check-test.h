#include <stdio.h>

#define TEST(testname){printf(#testname); testname(); printf(": OK\n");}

 void m_free(void * ptr);

struct s1
{
    int a;
    int b;
};

struct s2
{
    int n;
    char * ar1;
    int * ar2;
    struct s1 * nested_st;
    struct s1 ** array_s1;
};
