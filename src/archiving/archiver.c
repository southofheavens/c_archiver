#include <archiver.h>
#include <lz77.h>
#include <Huffman.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

darray *
serialize_token
(
    lz77_token tok
)
{
    darray *buffer = darray_create(sizeof(uint8_t));

    uint8_t curr_byte = 0;
    uint8_t byte_index = 0;
    
    for (int i = sizeof(tok.shift) * CHAR_BIT - 1; i >= 0; --i)
    {
        bool bit = (tok.shift >> i) & 1;
        if (bit) {
            curr_byte |= (1 << (CHAR_BIT - 1 - byte_index));
        }
        
        if (++byte_index == CHAR_BIT)
        {
            darray_append(buffer, curr_byte);
            byte_index = 0;
            curr_byte = 0;
        }
    }
    for (int i = sizeof(tok.length) * CHAR_BIT - 1; i >= 0; --i)
    {
        bool bit = (tok.length >> i) & 1;
        if (bit) {
            curr_byte |= (1 << (CHAR_BIT - 1 - byte_index));
        }
        
        if (++byte_index == CHAR_BIT)
        {
            darray_append(buffer, curr_byte);
            byte_index = 0;
            curr_byte = 0;
        }
    }
    for (int i = sizeof(tok.letter) * CHAR_BIT - 1; i >= 0; --i)
    {
        bool bit = (tok.letter >> i) & 1;
        if (bit) {
            curr_byte |= (1 << (CHAR_BIT - 1 - byte_index));
        }
        
        if (++byte_index == CHAR_BIT)
        {
            darray_append(buffer, curr_byte);
            byte_index = 0;
            curr_byte = 0;
        }
    }
    if (tok.eom) {
        curr_byte |= 1;
    }
    darray_append(buffer, curr_byte);

    return buffer;
}

lz77_token
deserialize_token
(
    darray *serialized_token
)
{
    lz77_token tok;
    memset(&tok, 0, sizeof(tok));

    uint8_t byte_index = 0;
    uint8_t serialized_token_index = 0;
    uint8_t curr_byte = darray_at(serialized_token, serialized_token_index, uint8_t);

    for (int i = sizeof(tok.shift) * CHAR_BIT - 1; i >= 0; --i)
    {
        bool bit = (curr_byte >> (CHAR_BIT - 1 - byte_index)) & 1;
        if (bit) {
            tok.shift += (int)pow(2,i);
        }

        if (++byte_index == CHAR_BIT)
        {
            byte_index = 0;
            serialized_token_index++;
            curr_byte = darray_at(serialized_token, serialized_token_index, uint8_t);
        }
    }
    for (int i = sizeof(tok.length) * CHAR_BIT - 1; i >= 0; --i)
    {
        bool bit = (curr_byte >> (CHAR_BIT - 1 - byte_index)) & 1;
        if (bit) {
            tok.length += (int)pow(2,i);
        }

        if (++byte_index == CHAR_BIT)
        {
            byte_index = 0;
            serialized_token_index++;
            curr_byte = darray_at(serialized_token, serialized_token_index, uint8_t);
        }
    }
    for (int i = sizeof(tok.letter) * CHAR_BIT - 1; i >= 0; --i)
    {
        bool bit = (curr_byte >> (CHAR_BIT - 1 - byte_index)) & 1;
        if (bit) {
            tok.letter += (int)pow(2,i);
        }

        if (++byte_index == CHAR_BIT)
        {
            byte_index = 0;
            serialized_token_index++;
            curr_byte = darray_at(serialized_token, serialized_token_index, uint8_t);
        }
    }
    if (curr_byte & 1) {
        tok.eom = true;
    }

    return tok;
}

bitset *
compress
(
    darray *bytes
)
{
    darray *tokens = lz77_encode(bytes);

    darray *serialized_tokens = darray_create(sizeof(uint8_t));

    for (size_t i = 0; i < darray_size(tokens); ++i) 
    {
        darray *serialized_token = serialize_token(darray_at(tokens, i, lz77_token));
        
        for (size_t j = 0; j < darray_size(serialized_token); ++j)
        {
            uint8_t byte = darray_at(serialized_token, j, uint8_t);
            darray_append(serialized_tokens, byte);
        }

        darray_free(serialized_token);
    }

    darray_free(tokens);

    bitset *result = huffman_tree_encode(serialized_tokens);

    darray_free(serialized_tokens);

    return result;
}

darray *
unpack
(
    bitset *bits
)
{
    darray *serialized_tokens = huffman_tree_decode(bits); // <- проблема тут

    darray *tokens = darray_create(sizeof(lz77_token));

    for (size_t i = 0; i < darray_size(serialized_tokens); i += 6)
    {
        darray_iterator begin = darray_begin(serialized_tokens);
        darray_iterator_advance(begin, i, right);
        darray_iterator end = begin;
        darray_iterator_advance(end, sizeof(lz77_token), right);
        darray *serialized_token_darr = darray_create_iter(begin,end);
        lz77_token tok = deserialize_token(serialized_token_darr); 
        darray_free(serialized_token_darr);
        darray_append(tokens, tok);
    }

    darray_free(serialized_tokens);

    darray *result = lz77_decode(tokens);

    darray_free(tokens);

    return result;
}
