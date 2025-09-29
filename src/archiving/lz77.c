#include <lz77.h>
#include <memory.h>

// #define BUFFER_SIZE 2048
#define NPOS ULLONG_MAX

size_t
lz77_search_byte_seq_in_buffer
(
    const darray *buffer,
    const darray *byte_seq
)
{
    if (darray_size(buffer) < darray_size(byte_seq)) {
        return NPOS;
    }

    size_t text_index = darray_size(buffer) - 1;
    for (size_t i = 0; i < darray_size(buffer) - darray_size(byte_seq) + 1; ++i)
    {
        size_t curr_text_index = text_index;
        size_t pattern_index = darray_size(byte_seq) - 1;
        for (size_t j = 0; j < darray_size(byte_seq); ++j)
        {
            if (darray_at(buffer, curr_text_index, uint8_t) != 
                darray_at(byte_seq, pattern_index, uint8_t)) 
            {
                break;
            }
            curr_text_index--;
            pattern_index--;
        }

        if (pattern_index == NPOS) 
        /** 
         * После успешного сравнения переменная pattern_index, содержащая 0, после декремента
         * переполнилась и стала содержать значение, которое равно NPOS
         **/
        {
            return curr_text_index + 1;
        } 

        text_index--;
    }

    return NPOS;
}

lz77_token lz77_search_for_occurrence
(
    const darray *buffer,
    const darray *byte_seq,
    size_t       *index
)
{
    lz77_token token;
    memset(&token, 0, sizeof(token));

    // Индекс начала текущей последовательности
    //size_t start_index = dstring_length((dstring *)buffer) < BUFFER_SIZE ? *index : BUFFER_SIZE + 1;
    size_t start_index = *index;
    // Текущая последовательность байт
    //dstring *curr_substr = dstring_create_empty();
    darray *curr_byte_seq = darray_create(sizeof(uint8_t));
    // Индекс нахождения curr_byte_seq в буфере
    size_t index_of_occurrence;

    size_t i = 0;
    do    
    {
        uint8_t curr_byte = darray_at(byte_seq, *index, uint8_t);
        darray_append(curr_byte_seq, curr_byte);

        size_t index_of_curr_occur;
        if ((index_of_curr_occur = 
            lz77_search_byte_seq_in_buffer(buffer, curr_byte_seq)) != NPOS)
        {
            index_of_occurrence = index_of_curr_occur;

            token.shift = start_index - index_of_occurrence;
            token.length = darray_size(curr_byte_seq);

            if (*index == darray_size(byte_seq) - 1) {
                token.eom = true;
            }
            else if (i == darray_size(buffer) - 1) {
                token.letter = darray_at(byte_seq, ++(*index), uint8_t);
            }
        }
        else 
        {
            token.letter = curr_byte;
            (*index)++;
            break;
        }

        (*index)++;
        i++;
    }
    while (i < darray_size(buffer) 
        && *index < darray_size(byte_seq));

    darray_free(curr_byte_seq);

    //TODO continue - закончил здесь, надо перепроверить функцию и продолжать переписывать на darray

    return token;
}

darray *
lz77_encode
(
    const dstring *dstr
)
{
    dstring *buffer = dstring_create_empty();
    darray *tokens = darray_create(sizeof(lz77_token));
    
    size_t i = 0;
    while (i < dstring_length((dstring *)dstr))
    {
        lz77_token another_token = lz77_search_for_occurrence(buffer, dstr, &i);

        // printf("%hu - %hu - %c - %d\n", another_token.shift, another_token.length, another_token.letter, another_token.eom);

        darray_append(tokens, another_token);

        dstring *substr = 
            dstring_substr(dstr, i - (another_token.length + 1), another_token.length + 1);
        dstring_concat(buffer, substr);

        dstring_free(substr);

        // if (dstring_length(buffer) > BUFFER_SIZE) 
        // {
        //     dstring *new_buffer = 
        //         dstring_substr(buffer, dstring_length(buffer) - BUFFER_SIZE - 1, BUFFER_SIZE);
        //     dstring_free(buffer);
        //     buffer = new_buffer;
        // }
    }

    dstring_free(buffer);
    return tokens;
}

dstring *
lz77_decode
(
    const darray *tokens
)
{
    dstring *result = dstring_create_empty(); 

    for (size_t i = 0; i < darray_size((darray *)tokens); ++i)
    {
        lz77_token tok = darray_at((darray *)tokens, i, lz77_token);
        dstring *curr_str =
            dstring_substr(result, dstring_length(result) - tok.shift, tok.length);

        if (tok.eom == false) {
            dstring_append(curr_str, tok.letter);
        }

        dstring_concat(result, curr_str);
        dstring_free(curr_str);
    }

    return result;
}