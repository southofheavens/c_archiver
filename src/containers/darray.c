#include "darray.h"

#include <stdlib.h>
#include <errno.h>
#include <memory.h>
#include <stddef.h>

#define DARRAY_INIT_CAPACITY 8


/*  */


typedef struct darray
{
    void *elems;
    size_t capacity;
    size_t size;
    size_t type_size;
} darray;

// TODO возможно указать на дэррэй не нужен, пересмотреть реализацию (в АВЛ дереве тоже)
typedef struct darray_iterator_impl
{
    darray *master_darr;
    void   *elem;
} darray_iterator_impl;


/* */


darray *
darray_create
(
    size_t type_size
)
{
    darray *darr = (darray *)malloc(sizeof(darray));
    if (darr == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    darr->elems = NULL;
    darr->capacity = 0;
    darr->size = 0;
    darr->type_size = type_size;

    return darr;
}

darray *
darray_create_iter
(
    darray_iterator begin,
    darray_iterator end
)
{
    darray_iterator_impl begin_impl = *((darray_iterator_impl *)&begin);
    darray_iterator_impl end_impl = *((darray_iterator_impl *)&end);

    ptrdiff_t diff = ((char *)(end_impl.elem) - (char *)(begin_impl.elem)) / begin_impl.master_darr->type_size;

    darray *darr = (darray *)malloc(sizeof(darray));
    if (darr == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    darr->type_size = begin_impl.master_darr->type_size;
    darr->size = darr->capacity = diff;
    darr->elems = malloc(darr->capacity * darr->type_size);
    if (darr->elems == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }   

    memcpy(darr->elems, begin_impl.elem, darr->capacity * darr->type_size);

    return darr;
}

void 
darray_free
(
    darray *darr
)
{
    if (darr->elems) {
        free(darr->elems);
    }
    free(darr);
}

void 
_darray_append
(
    darray *darr, 
    void   *elem
)
{
    if (darr->size == darr->capacity)
    {
        if (darr->capacity != 0)
        {
            darr->capacity *= 2;
            darr->elems = realloc(darr->elems, darr->capacity * darr->type_size);
        }
        else 
        {
            darr->capacity = DARRAY_INIT_CAPACITY;
            darr->elems = malloc(darr->capacity * darr->type_size);
        }

        if (darr->elems == NULL)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
    }

    memcpy(((char *)(darr->elems)) + (darr->size * darr->type_size), elem, darr->type_size);

    darr->size++;
}

void *
_darray_at
(
    darray *darr, 
    size_t  index
)
{
    void *pointer_to_elem = ((char *)(darr->elems)) + (index * darr->type_size);

    return pointer_to_elem;
}

size_t 
darray_size
(
    darray *darr
)
{
    return darr->size;
}

void 
darray_resize
(
    darray *darr, 
    size_t  new_size
)
{
    if (new_size > darr->size)
    {
        if (new_size > darr->capacity)
        {
            darr->capacity = new_size;

            darr->elems = realloc(darr->elems, darr->capacity * darr->type_size);
            if (darr->elems == NULL)
            {
                fprintf(stderr, "%s\n", strerror(errno));
                exit(1);
            }
        }

        memset((char *)(darr->elems) + (darr->size * darr->type_size), 0, (new_size - darr->size) * darr->type_size);
    }

    darr->size = new_size;
}

size_t 
darray_capacity
(
    darray *darr
)
{   
    return darr->capacity;
}

void 
darray_reserve
(
    darray *darr, 
    size_t  new_capacity
)
{
    if (new_capacity > darr->capacity)
    {
        darr->capacity = new_capacity;

        darr->elems = realloc(darr->elems, darr->capacity * darr->type_size);
        if (darr->elems == NULL)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
    }
}

void 
darray_erase
(
    darray *darr, 
    size_t  index
)
{
    if (darr->size > 0)
    {
        if (index == darr->size - 1)
        {
            darr->size--;
        }
        else if (index < darr->size - 1)
        {
            void *dest = ((char *)(darr->elems) + darr->type_size * index);
            void *src = ((char *)(darr->elems) + (darr->type_size * (index + 1)));
            size_t bytes_to_move = (darr->size - index - 1) * darr->type_size;

            memmove(dest, src, bytes_to_move);
            
            darr->size--;
        }
        else
        {
            fprintf(stderr, "darray_erase: array index out of range\n");
            exit(1);
        }
    }
    else
    {
        fprintf(stderr, "darray_erase: array is empty but an attempt was made to delete an element \n");
        exit(1);
    }
}

darray_iterator
_darray_find
(
    darray *darr,
    void   *elem,
    int   (*compare_func)(const void *first, const void *second)
)
{
    void *start_pointer;
    for 
    (
        start_pointer = darr->elems; 
        start_pointer != (void *)((char *)darr->elems + darr->type_size * darr->size); 
        start_pointer = (void *)((char *)(start_pointer) + darr->type_size)
    )
    {
        if (compare_func(start_pointer, elem) == 0) {
            break;
        }
    }

    darray_iterator_impl it_impl;
    it_impl = (darray_iterator_impl)
    {
        .master_darr = darr,
        .elem = start_pointer
    };
    return *((darray_iterator *)(&it_impl));
}

darray_iterator
darray_begin
(
    darray *darr
)
{
    darray_iterator_impl it_impl;
    it_impl = (darray_iterator_impl)
    {
        .master_darr = darr,
        .elem = darr->elems
    };
    return *((darray_iterator *)(&it_impl));
}

darray_iterator
darray_last
(
    darray *darr
)
{
    darray_iterator_impl it_impl;
    it_impl = (darray_iterator_impl)
    {
        .master_darr = darr,
        .elem = (darr->size == 0 ? darr->elems : (void *)((char *)(darr->elems)) + darr->type_size * (darr->size - 1))
    };

    return *((darray_iterator *)(&it_impl));
}

darray_iterator
darray_end
(
    darray *darr
)
{
    darray_iterator_impl it_impl;
    it_impl = (darray_iterator_impl)
    {
        .master_darr = darr,
        .elem = (void *)((char *)(darr->elems) + darr->type_size * darr->size)
    };

    return *((darray_iterator *)(&it_impl));
}

void
_darray_iterator_advance
(
    darray_iterator      *it,
    size_t                offset,
    darray_iterator_side  side
)
{
    darray_iterator_impl *it_impl;
    it_impl = (darray_iterator_impl *)it;

    if (side == (darray_iterator_side){left}) {
        it_impl->elem = (void *)((char *)(it_impl->elem) - it_impl->master_darr->type_size * offset);
    }
    else {
        it_impl->elem = (void *)((char *)(it_impl->elem) + it_impl->master_darr->type_size * offset);
    }
}

void *
_darray_iterator_get_value
(
    const darray_iterator it
)
{
    return ((darray_iterator_impl *)(&it))->elem;   
}

int
darray_iterator_compare
(
    const darray_iterator it1,
    const darray_iterator it2
)
{
    darray_iterator_impl first_impl, second_impl;
    first_impl = *((darray_iterator_impl *)(&it1));
    second_impl = *((darray_iterator_impl *)(&it2));

    return first_impl.elem - second_impl.elem;
}

void
_darray_iterator_swap
(
    darray_iterator *it1,
    darray_iterator *it2
)
{
    darray_iterator_impl *impl1 = (darray_iterator_impl *)it1;
    darray_iterator_impl *impl2 = (darray_iterator_impl *)it2;
    size_t type_size = impl1->master_darr->type_size;

    uint8_t temp[type_size];

    memcpy(temp, impl1->elem, type_size);
    memcpy(impl1->elem, impl2->elem, type_size);
    memcpy(impl2->elem, temp, type_size);
}

void *
_darray_data
(
    darray *darr
)
{
    return darr->elems;
}