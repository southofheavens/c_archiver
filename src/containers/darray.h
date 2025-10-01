/**
 * Динамический массив
 * 
 * Функции не содержат предусловий, проверяющих указатели на неравенство NULL. 
 * Ответственность за это перекладывается на пользователя библиотеки.
 * 
 * Undefined behavior:
 * - Сравнение итераторов, принадлежащих разным контейнерам
 */

#ifndef __DARRAY_H__
#define __DARRAY_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct darray darray;

struct __darray_iterator
{
    int16_t dummy1;
    int16_t dummy2;
    int16_t dummy3;
    int16_t dummy4;
    int16_t dummy5;
    int16_t dummy6;
    int16_t dummy7;
    int16_t dummy8;
};

typedef struct __darray_iterator _darray_iterator;
typedef _darray_iterator darray_iterator;

/**
 * Перечисление необходимо для функции darray_iterator_advance
 */
typedef enum darray_iterator_side {left, right} darray_iterator_side;

// TODO
// typedef enum darray_iterator_route {backward, forward} darray_iterator_route;

/**
 * Создаёт динамический массив и возвращает указатель на него
 * Принимает в качестве аргумента размер типа элементов, которые будут храниться в массиве
 */
darray *
darray_create
(
    size_t type_size
);

/**
 * 
 */
darray *
darray_create_iter
(
    darray_iterator begin,
    darray_iterator end
);

/**
 * Освобождает ресурсы, занятые динамическим массивом
 * Принимает в качестве аргумента указатель на динамический массив
 */
void 
darray_free
(
    darray *darr
);

/**
 * Добавляет элемент в конец динамического массива
 * Принимает в качестве аргументов указатель на динамический массив и элемент
 * Элемент должен быть lvalue (иметь адрес)
 */
#define darray_append(darr,elem) _darray_append(darr,(&(elem)))

void 
_darray_append
(
    darray *darr, 
    void   *pointer_to_elem
);

/**
 * Возвращает элемент динамического массива по индексу
 * Принимает в качестве аргументов указатель на динамический массив, тип элементов, которые хранятся 
 * в динамическом массиве и индекс
 */
#define darray_at(darr,ind,type) (*((type *)(_darray_at(darr,ind))))

void *
_darray_at
(
    darray *darr, 
    size_t  index
);

/**
 * 
 */
#define darray_set(darr,index,elem) _darray_set(darr,index,&(elem))

void
_darray_set
(
    darray *darr,
    size_t  index,
    void   *new_value 
);

/**
 * Возвращает размер динамического массива
 * Принимает в качестве аргумента указатель на динамический массив
 */
size_t 
darray_size
(
    darray *darr
);

/**
 * Изменяет размер динамического массива, при этом ёмкость никак не изменяется
 * Если размер больше текущего, то новые элементы обнуляются с помощью memset
 * Принимает в качестве аргумента указатель на динамический массив и новый размер
 */
void 
darray_resize
(
    darray *darr, 
    size_t  new_size
);

/**
 * Возвращает ёмкость динамического массива
 * Принимает в качестве аргумента указатель на динамический массив
 */
size_t 
darray_capacity
(
    darray *darr
);

/**
 * Изменяет ёмкость динамического массива. Если новая ёмкость меньше текущей, то ничего не меняется
 * Принимает в качестве аргумента указатель на динамический массив и новую ёмкость
 */
void 
darray_reserve
(
    darray *darray, 
    size_t  new_capacity
);

/**
 * Удаляет элемент по индексу из динамического массива
 * Принимает в качестве аргументов указатель на динамический массив и индекс
 */
void 
darray_erase
(
    darray *darr, 
    size_t  ind
);

/**
 * 
 */
#define darray_find(darr,elem,compare_func) _darray_find(darr,&(elem),compare_func)

darray_iterator
_darray_find
(
    darray *darr,
    void   *elem,
    int   (*compare_func)(const void *first, const void *second)
);

/* */

/**
 * 
 */
darray_iterator
darray_begin
(
    darray *darr
);

/**
 * 
 */
darray_iterator
darray_last
(
    darray *darr
);

/**
 * 
 */
darray_iterator
darray_end
(
    darray *darr
);

/**
 * 
 */
#define darray_iterator_advance(it,offset,side) _darray_iterator_advance(&(it),offset,side)

void
_darray_iterator_advance
(
    darray_iterator      *it,
    size_t                offset,
    darray_iterator_side  side
);

/**
 * 
 */
#define darray_iterator_get_value(it,type) (*((type *)(_darray_iterator_get_value(it))))

void *
_darray_iterator_get_value
(
    const darray_iterator it
);

/**
 * Сравнивает два итератора и возвращает:
 * - отрицательное число, если it1 находится перед it2 
 * - 0,  если итераторы указывают на один элемент
 * - положительное число, если it1 находится после it2
 */
long int
darray_iterator_compare
(
    const darray_iterator it1,
    const darray_iterator it2
);

/**
 * 
 */
#define darray_iterator_swap(it1,it2) _darray_iterator_swap(&(it1),&(it2))

void
_darray_iterator_swap
(
    darray_iterator *it1,
    darray_iterator *it2
);

/**
 * 
 */
#define darray_data(darr, type) ((type *)(_darray_data(darr)))

void *
_darray_data
(
    darray *darr
);

#endif