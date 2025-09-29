#include <bitset.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

typedef struct bitset
{
    int8_t *data;
    size_t   size;
} bitset;

bitset *
bitset_create
(
    void
)
{
    bitset *bits = (bitset *)malloc(sizeof(bitset));
    if (bits == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    bits->data = (int8_t *)malloc(sizeof(int8_t));
    if (bits->data == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    bits->size = 0;

    return bits;
}

void
bitset_free
(
    bitset *bits
)
{
    free(bits->data);
    free(bits);
}

void
bitset_append
(
    bitset *bits,
    bool    bit
)
{
    if (bits->size != 0 && bits->size % CHAR_BIT == 0)
    {
        bits->data = (int8_t *)realloc(bits->data, bits->size / CHAR_BIT + sizeof(int8_t));
        if (bits->data == NULL)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
    }
    
    if (bit)
    {
        int8_t *curr_block = bits->data + bits->size / CHAR_BIT;
        *curr_block = *curr_block | (1 << (CHAR_BIT - bits->size % 8 - 1));
    }

    (bits->size)++;
}

const int8_t *
bitset_data
(
    bitset *bits
)
{
    return bits->data;
}

size_t
bitset_size
(
    bitset *bits
)
{
    return bits->size;
}

bool
bitset_at
(
    bitset *bits,
    size_t  index
)
{
    int8_t *curr_block = bits->data + index / CHAR_BIT;
    return (*curr_block >> (CHAR_BIT - index % CHAR_BIT - 1)) & 1;
}