#include <archiver.h>
#include <lz77.h>
#include <Huffman.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <errno.h>

bitset *
serialize_token
(
    lz77_token tok
)
{
    bitset *buffer = bitset_create();

    for (int i = sizeof(uint16_t) * CHAR_BIT - 1; i >= 0; --i)
    {
        int bit = (tok.shift >> i) & 1;
        bitset_append(buffer, bit);
    }
    for (int i = sizeof(uint16_t) * CHAR_BIT - 1; i >= 0; --i)
    {
        int bit = (tok.length >> i) & 1;
        bitset_append(buffer, bit);
    }
    for (int i = sizeof(char) * CHAR_BIT - 1; i >= 0; --i)
    {
        int bit = (tok.letter >> i) & 1;
        bitset_append(buffer, bit);
    }
    for (int i = sizeof(bool) * CHAR_BIT - 2; i >= 0; --i)
    {
        bitset_append(buffer, 0);
    }
    bitset_append(buffer, (tok.eom & 1) == 1 ? 1 : 0);

    return buffer;
}

lz77_token
deserialize_token
(
    bitset *serialized_token
)
{
    lz77_token tok;
    memset(&tok, 0, sizeof(tok));

    size_t j = 0;
    for (int i = sizeof(uint16_t) * CHAR_BIT - 1; i >= 0; --i)
    {
        if (bitset_at(serialized_token, j++) == 1) {
            tok.shift += pow(2,i);
        }
    }
    for (int i = sizeof(uint16_t) * CHAR_BIT - 1; i >= 0; --i)
    {
        if (bitset_at(serialized_token, j++) == 1) {
            tok.length += pow(2,i);
        }
    }
    for (int i = sizeof(char) * CHAR_BIT - 1; i >= 0; --i)
    {
        if (bitset_at(serialized_token, j++) == 1) {
            tok.letter += pow(2,i);
        }
    }
    for (int i = sizeof(bool) * CHAR_BIT - 1; i >= 0; --i)
    {
        if (bitset_at(serialized_token, j++) == 1) {
            tok.eom += pow(2,i);
        }
    }

    return tok;
}

dstring *
bitset_to_string
(
    bitset *bits
)
{
    const char *bits_data = (const char *)bitset_data(bits);

    char *bits_data_null_terminated = 
        (char *)malloc((bitset_size(bits) / CHAR_BIT) + 
            (bitset_size(bits) % CHAR_BIT == 0 ? 0 : 1) + sizeof(char));
    if (bits_data_null_terminated == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    memcpy(bits_data_null_terminated, bits_data, 
        (bitset_size(bits) / CHAR_BIT) + (bitset_size(bits) % CHAR_BIT == 0 ? 0 : 1));

    dstring *result = dstring_create_empty();
    for 
    (
        size_t i = 0; 
        i < (bitset_size(bits) / CHAR_BIT) + (bitset_size(bits) % CHAR_BIT == 0 ? 0 : 1); 
        ++i
    ) 
    {
        dstring_append(result, bits_data_null_terminated[i]);
    }

    free(bits_data_null_terminated);

    return result;
}

bitset *
string_to_bitset
(
    dstring *dstr
)
{
    bitset *bits = bitset_create();

    for (size_t i = 0; i < dstring_length(dstr); ++i)
    {
        char curr_char = dstring_at(dstr, i);

        for (int j = CHAR_BIT - 1; j >= 0; --j) {
            bitset_append(bits, (curr_char >> j) & 1);
        }
    }
    
    return bits;
}

bitset *
compress
(
    dstring *bytes
)
{
    darray *tokens = lz77_encode(bytes);

    dstring *serialized_tokens = dstring_create_empty(); // TODO переписать на битсет

    for (size_t i = 0; i < darray_size(tokens); ++i) 
    {
        bitset *serialized_token = serialize_token(darray_at(tokens, i, lz77_token));
        dstring *string_serialized_token = bitset_to_string(serialized_token);
        dstring_concat(serialized_tokens, string_serialized_token);  // проблема здесь в ебаных строках, копирование не происходит потому что strcat сразу спотыкается об \0
        dstring_free(string_serialized_token);
        bitset_free(serialized_token);
    }

    for (size_t i = 0; i < dstring_length(serialized_tokens); ++i)
    {
        printf("%c\n", dstring_at(serialized_tokens, i));
    }

    bitset *result = huffman_tree_encode(serialized_tokens);

    return result;
}

dstring *
unpack
(
    bitset *bits
)
{
    dstring *string_serialized_tokens = huffman_tree_decode(bits);

    darray *tokens = darray_create(sizeof(lz77_token));

    for (size_t i = 0; i < dstring_length(string_serialized_tokens); i += 6)
    {
        dstring *serialized_token = dstring_substr(string_serialized_tokens, i,  
            sizeof(lz77_token));
        bitset *bits_of_serialized_token = string_to_bitset(serialized_token);
        dstring_free(serialized_token);
        lz77_token tok = deserialize_token(bits_of_serialized_token); 
        bitset_free(bits_of_serialized_token);
        darray_append(tokens, tok);
    }

    dstring *result = lz77_decode(tokens);

    return result;
}
