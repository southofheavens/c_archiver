#include <stdio.h>
#include <stdint.h>
#include <darray.h>
#include <utils.h>
#include <limits.h>
#include <lz77.h>
#include <math.h>
#include <memory.h>
#include <dstring.h>
#include <Huffman.h>
#include <string.h>
#include <stack.h>
#include <stdlib.h>
#include <dstring.h>
#include <lz77.h>
#include <archiver.h>
#include <bitset.h>
#include <assert.h>

int main(int argc, char *argv[])
{   
    printf("ИСХОДНЫЙ ТЕКСТ:\n\n");
    darray *darr = darray_create(sizeof(int8_t));

    int8_t c;
    while ((c = getchar()) != EOF)
    {
        darray_append(darr, c);
    }

    bitset *bits = compress(darr);

    darray *d = unpack(bits);

    printf("\n\nРАЗМЕР ТЕКСТА В БИТАХ: %lu\n\n", darray_size(darr) * CHAR_BIT);

    printf("\n\nРАЗЖАТЫЙ ТЕКСТ:\n");
    for (size_t i = 0; i < darray_size(d); ++i)
    {
        printf("%c", darray_at(d,i,char));
    }

    printf("\n\nРАЗМЕР ТЕКСТА В БИТАХ: %lu\n\n", bitset_size(bits));

    darray_free(darr);
    bitset_free(bits);
    darray_free(d);
    return 0;
}

// написать функцию чтобы можно было сделать как в плюсах vec[index] = new_value;
// функцию, которая сдвигает итератор и возвращает сдвинутый итератор

// когда мы идём к отцу, нужно после каждого увеличения frequency сравнивать соседние элементы

// ну крч в хаффмане неправильная вставка в дерево происходит - узлы неправильно отсортированы, надо пересматривать решение

