#ifndef __BITSET_H__
#define __BITSET_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct bitset bitset;

/**
 * 
 */
bitset *
bitset_create
(
    void
);

/**
 * 
 */
void
bitset_free
(
    bitset *bits
);

/**
 * 
 */
void
bitset_append
(
    bitset *bits,
    bool    bit
);

/**
 * 
 */
uint8_t *
bitset_data
(
    bitset *bits
);

/**
 * 
 */
size_t
bitset_size
(
    bitset *bits
);

#endif