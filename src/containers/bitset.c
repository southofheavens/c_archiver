#include <bitset.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define BITS_IN_A_BYTE 8

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
    if (bits->size != 0 && bits->size % BITS_IN_A_BYTE == 0)
    {
        bits->data = (int8_t *)realloc(bits->data, bits->size / BITS_IN_A_BYTE + sizeof(int8_t));
        if (bits->data == NULL)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
    }
    
    if (bit)
    {
        int8_t *curr_block = bits->data + bits->size / BITS_IN_A_BYTE;
        *curr_block = *curr_block | (1 << (BITS_IN_A_BYTE - bits->size % 8 - 1));
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
    int8_t *curr_block = bits->data + index / BITS_IN_A_BYTE;
    return (*curr_block >> (BITS_IN_A_BYTE - index % BITS_IN_A_BYTE - 1)) & 1;
}