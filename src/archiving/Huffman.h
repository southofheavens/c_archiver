#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

#include <darray.h>
#include <dstring.h>
#include <bitset.h>

/**
 * Кодирует последовательность байт BYTES и возвращает последовательность бит bitset
 */
bitset *
huffman_tree_encode
(
    const darray *bytes
);

/**
 * Декодирует последовательность бит BITS и возвращает последовательность байт 
 */
darray *
huffman_tree_decode
(
    const bitset *bits
);

#endif