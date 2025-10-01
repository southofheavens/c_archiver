#ifndef __LZ77_H__
#define __LZ77_H__

#include <stdint.h>
#include <stdbool.h>
#include <darray.h>
#include <dstring.h>

typedef struct lz77_token
{
    uint16_t shift;
    uint16_t length;
    char     letter;
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