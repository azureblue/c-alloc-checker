#ifndef ALLOC_CHECK_H
#define ALLOC_CHECK_H

#include <inttypes.h>
#include <setjmp.h>
#include <stdbool.h>
#include <stddef.h>

#define alloc_check_free_function m_free
#define alloc_check_alloc_function m_alloc

extern void alloc_check_free_function(void *);
extern void * alloc_check_alloc_function(size_t size);

struct alloc_check_context
{
    jmp_buf jmpbuf;
    const int size;
    volatile int idx;    
    volatile intptr_t * ptrs;
};

/*
 * This macro implicitly declares an array of length buff_size (variable-length array), 
 * alloc_check_context struct and sets jmp_buf in the local scope that will be used 
 * to track and free pointers that are passed the *check* macro.
 */
#define checked_allocation(buf_size)                                                                                                \
volatile intptr_t __alloc_check_ptrs[buf_size];                                                                                     \
struct alloc_check_context __alloc_check_context = {.idx = 0, .size = buf_size, .ptrs = __alloc_check_ptrs};                        \
if (setjmp(__alloc_check_context.jmpbuf)) return (alloc_check_free_pointers(&__alloc_check_context), NULL);

/*
 * Macros that *alter control flow* by causing the function, inside
 * which they have been used, to *return* NULL if the *ptr* expression
 * evaluates to NULL or alloc_check_alloc_function returns NULL (is case of check_alloc).
 * Before the return it frees all pointers that were previously passed to *check_ptr* 
 * or *check_alloc* in the context of the local scope.
 *
 * The use of this macro must be preceded by the use of *checked_allocation* 
 * in the context of the surrounding function.
 */
#define check_ptr(ptr)                                                                                                              \
    (alloc_check_idx_within_range(&__alloc_check_context), alloc_check_ptr(ptr, &__alloc_check_context))

/*
 * Passes alloc_check_alloc_function(size_t_s) to check_ptr(ptr).
 * 
 * @see check_ptr(ptr)
 */
#define check_alloc(size_t_s)                                                                                                       \
    check_ptr(alloc_check_alloc_function(size_t_s))

#define free_checked_ptrs()                                                                                                         \
    alloc_check_free_pointers(&__alloc_check_context)

void alloc_check_longjmp_clean(struct alloc_check_context *);

void alloc_check_idx_within_range(struct alloc_check_context *);

void * alloc_check_ptr(void * pointer, struct alloc_check_context *);

void alloc_check_free_pointers(struct alloc_check_context *);

#endif /* ALLOC_CHECK_H */
