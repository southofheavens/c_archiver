#ifndef __STACK_H__
#define __STACK_H__

#include <stdio.h>
#include <stdbool.h>

typedef struct _stack stack;

/**
 * 
 */
stack *
stack_create
(
    size_t type_size
);

/**
 * 
 */
#define stack_push(st,elem) _stack_push(st,&(elem))

void
_stack_push
(
    stack *st,
    void  *elem
);

/**
 * 
 */
#define stack_top(st,type) (*((type *)(_stack_top(st))))

void *
_stack_top
(
    stack *st
);

/**
 * 
 */
void
stack_pop
(
    stack *st
);

/**
 * 
 */
size_t
stack_size
(
    stack *st
);

/**
 * 
 */
void
stack_free
(
    stack *st
);

/**
 * 
 */
bool
stack_empty
(
    stack *st
);

#endif