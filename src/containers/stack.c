#include <stack.h>

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct stack_node stack_node;

struct stack_node
{
    void       *elem;
    stack_node *prev;
};

typedef struct _stack
{
    size_t      type_size;
    size_t      size;
    stack_node *top;
} stack;

/* */

stack *
stack_create 
(
    size_t type_size
)
{
    stack *st = (stack *)malloc(sizeof(stack));
    if (st == NULL)
    {
        perror("");
        exit(1);
    }

    *st = (stack)
    {
        .type_size = type_size,
        .top = NULL
    };

    return st;
}

void
_stack_push
(
    stack *st,
    void  *elem
)
{
    if (st == NULL || elem == NULL)
    {
        fprintf(stderr, "stack_push: передан нулевой указатель в качестве аргумента\n");
        exit(1);
    }

    stack_node *node = (stack_node *)malloc(sizeof(stack_node));
    if (node == NULL)
    {
        perror("");
        exit(1);
    }

    node->elem = malloc(st->type_size);
    if (node->elem == NULL)
    {
        perror("");
        exit(1);
    }

    memcpy(node->elem, elem, st->type_size);

    node->prev = st->top;

    st->top = node;
    st->size++;
}

void *
_stack_top
(
    stack *st
)
{
    if (st == NULL)
    {
        fprintf(stderr, "stack_top: передан нулевой указатель в качестве аргумента\n");
        exit(1);
    }

    return st->top->elem;
}

void
stack_pop
(
    stack *st
)
{
    if (st == NULL)
    {
        fprintf(stderr, "stack_pop: передан нулевой указатель в качестве аргумента\n");
        exit(1);
    }

    stack_node *temp = st->top->prev;

    free(st->top->elem);
    free(st->top);

    st->top = temp;
    st->size--;
}

size_t
stack_size
(
    stack *st
)
{
    if (st == NULL)
    {
        fprintf(stderr, "stack_size: передан нулевой указатель в качестве аргумента\n");
        exit(1);
    }

    return st->size; 
}

void
stack_free
(
    stack *st
)
{
    if (st == NULL)
    {
        fprintf(stderr, "stack_free: передан нулевой указатель в качестве аргумента\n");
        exit(1);
    }

    stack_node *curr_top = st->top;
    while (curr_top != NULL)
    {
        stack_node *temp = curr_top->prev;

        free(curr_top->elem);
        free(curr_top);

        curr_top = temp;
    }

    free(st);
}

bool
stack_empty
(
    stack *st
)
{
    if (st == NULL)
    {
        fprintf(stderr, "stack_empty: передан нулевой указатель в качестве аргумента\n");
        exit(1);
    }

    return (st->size == 0 ? true : false);
}