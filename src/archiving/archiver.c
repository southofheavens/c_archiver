#include <archiver.h>
#include <lz77.h>
#include <Huffman.h>
#include <math.h>
#include <memory.h>

#define BITS_IN_BYTE 8

// TODO переписать на биты
dstring *
serialize_token
(
    lz77_token tok
)
{
    dstring *buffer = dstring_create_empty();
    // TODO reserve

    for (int i = sizeof(uint16_t) * BITS_IN_BYTE - 1; i >= 0; --i)
    {
        int bit = (tok.shift >> i) & 1;
        dstring_append(buffer, bit == 0 ? '0' : '1');
    }
    for (int i = sizeof(uint16_t) * BITS_IN_BYTE - 1; i >= 0; --i)
    {
        int bit = (tok.length >> i) & 1;
        dstring_append(buffer, bit == 0 ? '0' : '1');
    }
    for (int i = sizeof(char) * BITS_IN_BYTE - 1; i >= 0; --i)
    {
        int bit = (tok.letter >> i) & 1;
        dstring_append(buffer, bit == 0 ? '0' : '1');
    }
    for (int i = sizeof(bool) * BITS_IN_BYTE - 2; i >= 0; --i)
    {
        dstring_append(buffer, '0');
    }
    dstring_append(buffer, (tok.eom & 1) == 1 ? '1' : '0');

    return buffer;
}

lz77_token
deserialize_token
(
    dstring *serialized_token
)
{
    lz77_token tok;
    memset(&tok, 0, sizeof(tok));

    size_t j = 0;
    for (int i = sizeof(uint16_t) * BITS_IN_BYTE - 1; i >= 0; --i)
    {
        if (dstring_at(serialized_token, j++) == '1') {
            tok.shift += pow(2,i);
        }
    }
    for (int i = sizeof(uint16_t) * BITS_IN_BYTE - 1; i >= 0; --i)
    {
        if (dstring_at(serialized_token, j++) == '1') {
            tok.length += pow(2,i);
        }
    }
    for (int i = sizeof(char) * BITS_IN_BYTE - 1; i >= 0; --i)
    {
        if (dstring_at(serialized_token, j++) == '1') {
            tok.letter += pow(2,i);
        }
    }
    for (int i = sizeof(bool) * BITS_IN_BYTE - 1; i >= 0; --i)
    {
        if (dstring_at(serialized_token, j++) == '1') {
            tok.eom += pow(2,i);
        }
    }

    return tok;
}

darray *
compress
(
    dstring *bytes
)
{
    darray *tokens = lz77_encode(bytes);

    for (size_t i = 0; i < darray_size(tokens); ++i)
    {
        lz77_token tok = darray_at(tokens, i, lz77_token);
    }

    dstring *serialized_tokens = dstring_create_empty();
    // TODO тут можно добавить dstring_reserve(serialized_tokens, darray_size(tokens) * 48);

    for (size_t i = 0; i < darray_size(tokens); ++i) 
    {
        dstring *serialized_token = serialize_token(darray_at(tokens, i, lz77_token));
        dstring_concat(serialized_tokens, serialized_token);
        dstring_free(serialized_token);
    }

    darray *result = huffman_tree_encode(serialized_tokens);

    return result;
}

dstring *
unpack
(
    darray *bits
)
{
    dstring *serialized_tokens = huffman_tree_decode(bits);

    darray *tokens = darray_create(sizeof(lz77_token));

    for (size_t i = 0; i < dstring_length(serialized_tokens); i += 48)
    {
        dstring *serialized_token = dstring_substr(serialized_tokens, i, 
            sizeof(lz77_token) * BITS_IN_BYTE);
        lz77_token tok = deserialize_token(serialized_token);
        //printf("%hu - %hu - %c - %d\n",tok.shift, tok.length, tok.letter, tok.eom);
        dstring_free(serialized_token);
        darray_append(tokens, tok);
    }

    dstring *result = lz77_decode(tokens);

    return result;
}

// hello -> lz_tokens -> serialized_tokens -> bits
// bits -> serialized_tokens -> lz_tokens -> hello