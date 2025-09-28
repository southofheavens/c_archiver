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
    uint8_t *data;
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

    bits->data = (uint8_t *)malloc(sizeof(uint8_t));
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
        bits->data = (uint8_t *)realloc(bits->data, bits->size / BITS_IN_A_BYTE + sizeof(uint8_t));
        if (bits->data == NULL)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
    }
    
    if (bit)
    {
        uint8_t *curr_block = bits->data + bits->size / BITS_IN_A_BYTE;
        *curr_block = *curr_block | (1 << (BITS_IN_A_BYTE - bits->size % 8 - 1));
    }

    (bits->size)++;
}

uint8_t *
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