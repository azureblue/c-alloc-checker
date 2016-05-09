#ifndef ALLOC_CHECK_H
#define ALLOC_CHECK_H

#include <setjmp.h>
#include <inttypes.h>

void m_free(void * ptr);

struct alloc_check_context
{
    jmp_buf jmpbuf;
    const int size;
    volatile int idx;    
    volatile intptr_t * ptrs;
};

/*
 * This macro implicitly sets a jmp_buf, declares a few variables 
 * and an array of pointers in the local scope that will be used 
 * to track and free pointers that are passed the *check* macro.
 */
#define checked_allocation(buf_size)                                                                                                \
volatile intptr_t __alloc_check_ptrs[buf_size];                                                                                     \
struct alloc_check_context __alloc_check_context = {.idx = 0, .size = buf_size, .ptrs = __alloc_check_ptrs};                        \
if (setjmp(__alloc_check_context.jmpbuf)) return (__alloc_check_free_pointers(&__alloc_check_context), NULL);

/*
 * Macro that *alters control flow* by causing the function, inside
 * which it has been used, to *return* NULL if the *pointer* expression
 * evaluates to NULL. Before the return it frees all pointers that
 * were previously passed to the *check* in the context of the local scope.
 *
 * This macro should be used as the rightmost expression of an assignment.
 *
 * The use of this macro must be preceded by the use of *checked_allocation;* 
 * in the context of the surrounding function.
 */
#define check(pointer)                                                                                                              \
(                                                                                                                                   \
    !__alloc_check_check_idx(&__alloc_check_context)                                                                                \
        ? __alloc_check_longjmp_clean(&__alloc_check_context) : __alloc_check_check_pointer(pointer, &__alloc_check_context)        \
)

static void * __alloc_check_longjmp_clean(struct alloc_check_context * context)
{
    longjmp(context->jmpbuf, 1);
    return NULL;
}

static int __alloc_check_check_idx(struct alloc_check_context * context)
{
    return context->idx < context->size;
}

static void * __alloc_check_check_pointer(void * pointer, struct alloc_check_context * context)
{    
    context->ptrs[context->idx++] = (intptr_t) pointer;
    if (!pointer)
        __alloc_check_longjmp_clean(context);
    
    return pointer;
}

static void __alloc_check_free_pointers(struct alloc_check_context * context)
{
    for (int i = 0; i < context->idx; i++)
    {
        m_free ((void *) context->ptrs[i]);
        context->ptrs[i] = (intptr_t) NULL;
    }
}

#endif /* ALLOC_CHECK_H */
