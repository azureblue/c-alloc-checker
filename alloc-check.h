#ifndef ALLOC_CHECK_H
#define ALLOC_CHECK_H

#include <setjmp.h>
#include <stdio.h>
#include <inttypes.h>

#define checked_allocator_default_size 100

void m_free(void * ptr);

#define clean_and_return_null() longjmp(__checked_allocator_jmp_buf, 1)

/*
 * This macro implicitly sets jmp_buf, declares a few variables 
 * and an array of pointers in the local scope that will be used 
 * to track and free pointers that are passed the *check* macro.
 */
#define checked_allocation(size)                                                                                            \
jmp_buf __checked_allocator_jmp_buf;                                                                                        \
const int __checked_allocator_size = (size);                                                                                \
volatile intptr_t __checked_allocator_ptrs[__checked_allocator_size];                                                       \
volatile int __checked_allocator_idx = 0;                                                                                   \
void * __checked_allocator_ptr;                                                                                             \
if (setjmp(__checked_allocator_jmp_buf))                                                                                    \
{                                                                                                                           \
    for (int i = 0; i < __checked_allocator_idx; i++)                                                                       \
        m_free ((void *) __checked_allocator_ptrs[i]);                                                                      \
    return NULL;                                                                                                            \
}

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
#define check(pointer)                                                                                                      \
(                                                                                                                           \
    __checked_allocator_ptr = __checked_allocator_idx == __checked_allocator_size ?                                         \
        (longjmp(__checked_allocator_jmp_buf, 1), NULL) : pointer,                                                          \
    __checked_allocator_ptrs[__checked_allocator_idx++] = (intptr_t) __checked_allocator_ptr,                               \
    !__checked_allocator_ptr ?                                                                                              \
        (longjmp(__checked_allocator_jmp_buf, 1), NULL) : __checked_allocator_ptr                                           \
)

#endif /* ALLOC_CHECK_H */
