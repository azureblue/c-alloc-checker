# C Alloc Checker

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
It's constructor funciton comes down to:
```C
struct my_struct * create_my_struct(int len)
{
    check_allocation(4);
    struct my_struct * my = check(malloc(sizeof (struct my_struct)));
    my->length = len;
    my->array_1 = check(malloc(sizeof (int) * len));
    my->array_2 = check(malloc(sizeof (float) * len));
    my->array_3 = check(malloc(sizeof (float) * len));
    
    return my;
}
```
instead of:
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
    
    if (!(my->array_3 = malloc(sizeof (float) * len)))
    {
        free(my->array_1);
        free(my->array_2);
        free(my);
        return NULL;
    }
    
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
    
    if (!(my->array_3 = malloc(sizeof (float) * len)))
        goto cleanup;
    
    return my;
    
cleanup:
    free(my->array_1);
    free(my->array_2);
    free(my->array_3);
    free(my);
    return NULL;
}
```

Of course it works with any other function returning a pointer:
```C
struct matrix_with_flexible_array
{
    int m;
    int n;
    double data[];
};

struct matrix_with_flexible_array * create_matrix(int m, int n)
{
    struct matrix_with_flexible_array * mat = 
        malloc(sizeof (struct matrix_with_flexible_array) + sizeof (double) * m * n);
    
    if (!mat)
        return NULL;    
    
    mat->m = m;
    mat->n = n;
    return mat;
}

struct my_struct
{
    int length;
    int * array_1;
    struct matrix_with_flexible_array * matrix_array[];
};

struct my_struct * create_my_struct(int m, int n, int len)
{
    check_allocation(2 + len);
    struct my_struct * my = check(malloc(sizeof (struct my_struct) 
        + sizeof (struct matrix_with_flexible_array *) * len));
    
    my->length = len;
    my->array_1 = check(malloc(sizeof (int) * len));
    
    for (int i = 0; i < len; i++)
        my->matrix_array[i] = check(create_matrix(m, n));
    
    return my;
}
```

