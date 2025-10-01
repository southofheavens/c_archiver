#ifndef __LZ77_H__
#define __LZ77_H__

#include <stdint.h>
#include <stdbool.h>
#include <darray.h>

typedef struct lz77_token
{
    uint32_t shift;
    uint32_t length;
    uint8_t     letter;
    bool     eom;
} lz77_token;

darray *
lz77_encode
(
    const darray *byte_seq 
);

darray *
lz77_decode
(
    const darray *tokens
);

#endif 