#include "alloc-check.h"

void alloc_check_longjmp_clean(struct alloc_check_context * context)
{
    longjmp(context->jmpbuf, 1);
}

void alloc_check_idx_within_range(struct alloc_check_context * context)
{
    if (context->idx >= context->size)
        alloc_check_longjmp_clean(context);
}

void * alloc_check_ptr(void * ptr, struct alloc_check_context * context)
{    
    context->ptrs[context->idx++] = (intptr_t) ptr;
    if (!ptr)
        alloc_check_longjmp_clean(context);
    
    return ptr;
}

void * alloc_check_alloc(size_t size, struct alloc_check_context * context)
{    
    return alloc_check_ptr(alloc_check_alloc_function(size), context);
}

void alloc_check_free_pointers(struct alloc_check_context * context)
{
    for (int i = 0; i < context->idx; i++)
        alloc_check_free_function ((void *) context->ptrs[i]);

    context->idx = 0;
}
