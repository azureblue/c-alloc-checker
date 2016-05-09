#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "alloc-check.h"
#include "alloc-check-test.h"

static int allocs = 0;
static int fail_alloc_after = 0;

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
    void * ma = malloc(size);
    
    if (!ma)
        exit(-1);
  
    return ma;
}

struct s1 * create_s1()
{
    checked_allocation(20);
    
    struct s1 * s1 = check(m_alloc(sizeof (struct s1)));
    s1->a = 0;
    s1->b = 1;
    
    return s1;
}

struct s2 * create_s2()
{
    checked_allocation(20);
    
    int n = 10;
    struct s2 * s2 = check(m_alloc(sizeof (struct s2)));
    s2->n = n;
    s2->ar1 = check(m_alloc(sizeof (char) * 2));
    s2->ar2 = check(m_alloc(sizeof (char) * 2));
    s2->nested_st = check(create_s1());
    s2->array_s1 = check(m_alloc(sizeof (struct s1 *) * n));
    
    for (int i = 0; i < n; i++)
        s2->array_s1[i] = check(create_s1());
    
    return s2;
}

void destroy_s2(struct s2 * st)
{
    if (!st)
        return;
    
    m_free(st->ar1);
    m_free(st->ar2);
    m_free(st->nested_st);
    
    for (int i = 0; i < st->n; i++)
        m_free(st->array_s1[i]);
    
    m_free(st->array_s1);
    m_free(st);
}

int ** create_arrays(int n, int check_buffer)
{
    checked_allocation(check_buffer);
    
    int ** arrays = check(m_alloc(sizeof (int *) * n));
    
    for (int i = 0; i < n; i++)
        arrays[i] = check(m_alloc(sizeof (int) * 10));
    
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
    struct s2 * s2;
    int n = 0;
    set_faill_alloc_after(0);
    do 
    {
        assert(allocs == 0);    
        s2 = create_s2();
    
        if (!s2)
            assert(allocs == 0);
        
        set_faill_alloc_after(++n);
    } while(!s2);
    
    destroy_s2(s2);    
    assert(allocs == 0);
}

void test_free_and_return_null_when_track_buffer_is_full()
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
    
    void * ptr = check(ca);
    assert(ptr);
    
    void * ptr2 = check(NULL);
    assert(0 == 1);
}

int main()
{
    TEST(test_allocs_and_frees_match);
    TEST(test_free_and_return_null_when_track_buffer_is_full);
    TEST(test_return_after_first_null);
    return 0;
}