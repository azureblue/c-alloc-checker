# c-alloc-checker

A little bit of hackery that makes the boilerplate code of checking the result of memory allocation (e.g. malloc) and freeing previously allocated resources when NULL unnecessary.

### The use case
Having a struct:
```C
struct my_struct
{
    int length;
    int * array_1;
    float * array_2;
    float * array_3;
};
```
It's constructor funciton may look like this:
```C
struct my_struct * create_my_struct(int len)
{
    check_alloc(4);
    struct my_struct * my = check_alloc(struct my_struct);
    my->length = len;
    my->array_1 = check_alloc_n(int, len);
    my->array_2 = check_alloc_n(float, len);
    // or
    my->array_3 = check_ptr(malloc(sizeof (float) * len));
    
    return my;
}
```

Another example:
```C
struct matrix_with_array
{
    int m;
    int n;
    double * data;
};

struct matrix_with_array * create_matrix(int m, int n)
{
    checked_alloc(2);
    struct matrix_with_array * mat = check_alloc(struct matrix_with_array);
    mat->data = check_alloc_n(double, m * n);
    mat->m = m;
    mat->n = n;
    return mat;
}

void destroy_matrix_with_array(void * m_ptr)
{
    struct matrix_with_array * m = m_ptr;
    if (!m)
        return;
    
    free(m->data);
    free(m);
}

struct my_struct
{
    int length;
    int * array_1;
    struct matrix_with_array * matrix_array[];
};

struct my_struct * create_my_struct(int m, int n, int len)
{
    checked_alloc(2 + len);
    struct my_struct * my = check_alloc_fam(struct my_struct, struct matrix_with_array, len);
    
    my->length = len;
    my->array_1 = check_alloc_n(int, len);
    
    for (int i = 0; i < len; i++)
        my->matrix_array[i] = check_ptr_destr(create_matrix(m, n), destroy_matrix_with_array);
    
    return my;
}
```

The old way:
```C
struct my_struct * create_my_struct_the_standard_way1(int len)
{
    struct my_struct * my = malloc(sizeof (struct my_struct));
    if (!my)
        return NULL;
    my->length = len;
    
    if (!(my->array_1 = malloc(sizeof (int) * len)))
    {
        free(my);
        return NULL;
    }
    
    if (!(my->array_2 = malloc(sizeof (float) * len)))
    {
        free(my->array_1);
        free(my);
        return NULL;
    }
    ...
   
    return my;
}

// or a goto-cleanup version 

struct my_struct * create_my_struct_the_standard_way2(int len)
{
    struct my_struct * my = malloc(sizeof (struct my_struct));
    if (!my)
        return NULL;
    
    my->length = len;
    my->array_1 = NULL; 
    my->array_2 = my->array_3 = NULL;
    
    if (!(my->array_1 = malloc(sizeof (int) * len)))
        goto cleanup;
    
    if (!(my->array_2 = malloc(sizeof (float) * len)))
        goto cleanup;
   ...
    
    return my;
    
cleanup:
    free(my->array_1);
    free(my->array_2);
    free(my->array_3);
    free(my);
    return NULL;
}
```