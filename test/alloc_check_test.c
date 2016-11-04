#include "../alloc_check.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define TEST(testname){printf(#testname); testname(); printf(": OK\n");}

struct simple_struct
{
    int a;
    int b;
};

struct complex_struct
{
    int n;
    char * ar1;
    int * ar2;
    struct simple_struct * nested_st;
    struct simple_struct ** array_st;
};

int allocs = 0;
int fail_alloc_after = 0;

void set_faill_alloc_after(int n_allocs)
{
    fail_alloc_after = n_allocs;
}

void m_free(void * ptr)
{
    if (ptr)
        allocs--;
    
    free(ptr);
}

void * m_alloc(size_t size)
{
    if (allocs == fail_alloc_after)
        return NULL;
  
    allocs++;
    
    void * mem = malloc(size);
    
    if (!mem)
        exit(-1);
  
    return mem;
}

struct simple_struct * create_simple_struct()
{
    checked_allocation(20);
    
    struct simple_struct * sim = check_alloc(sizeof (struct simple_struct));
    sim->a = 0;
    sim->b = 1;
    
    return sim;
}

struct complex_struct * create_complex_struct_with_check_alloc()
{
    checked_allocation(20);
    
    int n = 10;
    struct complex_struct * com = check_ptr(m_alloc(sizeof (struct complex_struct)));
    com->n = n;
    com->ar1 = check_alloc(sizeof (char) * 2);
    com->ar2 = check_ptr(m_alloc(sizeof (char) * 2));
    com->nested_st = check_ptr(create_simple_struct());
    com->array_st = check_alloc(sizeof (struct simple_struct *) * n);
    
    for (int i = 0; i < n; i++)
        com->array_st[i] = check_ptr(create_simple_struct());
    
    return com;
}

void destroy_complex_struct(struct complex_struct * st)
{
    if (!st)
        return;
    
    m_free(st->ar1);
    m_free(st->ar2);
    m_free(st->nested_st);
    
    for (int i = 0; i < st->n; i++)
        m_free(st->array_st[i]);
    
    m_free(st->array_st);
    m_free(st);
}

int ** create_arrays(int n, int check_buffer)
{
    checked_allocation(check_buffer);
    
    int ** arrays = check_ptr(m_alloc(sizeof (int *) * n));
    
    for (int i = 0; i < n; i++)
        arrays[i] = check_ptr(m_alloc(sizeof (int) * 10));
    
    return arrays;
}

void destroy_arrays(int ** arrays, int n)
{
    for (int i = 0; i < n; i++)
        m_free(arrays[i]);
    
    m_free(arrays);    
}

void test_allocs_and_frees_match()
{
    struct complex_struct * com;
    int n = 0;
    set_faill_alloc_after(0);    
    
    do 
    {
        assert(allocs == 0);    
        com = create_complex_struct_with_check_alloc();
    
        if (!com)
            assert(allocs == 0);
        
        set_faill_alloc_after(++n);
    } 
    while(!com);
    
    destroy_complex_struct(com);    
    assert(n > 1);
    assert(allocs == 0);
}

void test_allocs_and_frees_match_check_alloc()
{
    struct complex_struct * com;
    int n = 0;
    set_faill_alloc_after(0);    
    
    do 
    {
        assert(allocs == 0);    
        com = create_complex_struct_with_check_alloc();
    
        if (!com)
            assert(allocs == 0);
        
        set_faill_alloc_after(++n);
    } 
    while(!com);
    
    destroy_complex_struct(com);
    assert(n > 1);
    assert(allocs == 0);
}


void test_free_and_return_null_when_ptr_buffer_is_full()
{
    set_faill_alloc_after(100);
    assert(allocs == 0);
    
    assert(create_arrays(2, 1) == NULL);
    assert(allocs == 0);
    
    int ** arrs = create_arrays(2, 3);
    assert(arrs != NULL);
    
    destroy_arrays(arrs, 2);
    assert(allocs == 0);
}

void * test_return_after_first_null()
{    
    checked_allocation(5);
    
    char * ca = m_alloc(sizeof (char) * 2);
    
    void * ptr = check_ptr(ca);
    assert(ptr);
    
    void * ptr2 = check_ptr(NULL);
    
    // we shouldn't be here
    assert(0);
    
    return ptr2;
}

int main()
{    
    TEST(test_allocs_and_frees_match);
    TEST(test_return_after_first_null);
    TEST(test_free_and_return_null_when_ptr_buffer_is_full);
    
    return 0;
}
