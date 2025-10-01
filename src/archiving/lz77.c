#include <lz77.h>
#include <memory.h>
#include <limits.h>

#define NPOS        ULLONG_MAX
#define BUFFER_SIZE 2048

size_t
lz77_search_byte_seq_in_buffer
(
    const darray *buffer,
    const darray *byte_seq
)
{
    if (darray_size((darray *)buffer) < darray_size((darray *)byte_seq)) {
        return NPOS;
    }

    size_t text_index = darray_size((darray *)buffer) - 1;
    for (size_t i = 0; i < darray_size((darray *)buffer) - darray_size((darray *)byte_seq) + 1; ++i)
    {
        size_t curr_text_index = text_index;
        size_t pattern_index = darray_size((darray *)byte_seq) - 1;
        for (size_t j = 0; j < darray_size((darray *)byte_seq); ++j)
        {
            if (darray_at((darray *)buffer, curr_text_index, uint8_t) != 
                darray_at((darray *)byte_seq, pattern_index, uint8_t)) 
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
    size_t start_index = *index;
    // Текущая последовательность байт
    darray *curr_byte_seq = darray_create(sizeof(uint8_t));
    // Индекс нахождения curr_byte_seq в буфере
    size_t index_of_occurrence;

    size_t i = 0;
    do    
    {
        uint8_t curr_byte = darray_at((darray *)byte_seq, *index, uint8_t);
        darray_append(curr_byte_seq, curr_byte);

        size_t index_of_curr_occur;
        if ((index_of_curr_occur = 
            lz77_search_byte_seq_in_buffer((darray *)buffer, curr_byte_seq)) != NPOS)
        {
            index_of_occurrence = index_of_curr_occur;

            token.shift = start_index - index_of_occurrence;
            token.length = darray_size(curr_byte_seq);

            if (*index == darray_size((darray *)byte_seq) - 1) {
                token.eom = true;
            }
            else if (i == darray_size((darray *)buffer) - 1) {
                token.letter = darray_at((darray *)byte_seq, ++(*index), uint8_t);
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
    while (i < darray_size((darray *)buffer) 
        && *index < darray_size((darray *)byte_seq));

    darray_free(curr_byte_seq);

    return token;
}

darray *
lz77_encode
(
    const darray *byte_seq
)
{
    darray *buffer = darray_create(sizeof(uint8_t));
    darray *tokens = darray_create(sizeof(lz77_token));
    
    size_t i = 0;
    while (i < darray_size((darray *)byte_seq))
    {
        lz77_token another_token = lz77_search_for_occurrence(buffer, byte_seq, &i);

        darray_append(tokens, another_token);

        darray_iterator begin = darray_begin((darray *)byte_seq);
        darray_iterator_advance(begin, i - (another_token.length + 1), right);
        darray_iterator end = begin;
        darray_iterator_advance(end, another_token.length + 1, right);
        darray *consecutive_bytes = darray_create_iter(begin, end);
        
        for (size_t j = 0; j < darray_size(consecutive_bytes); ++j)
        {
            uint8_t byte = darray_at(consecutive_bytes, j, uint8_t);
            darray_append(buffer, byte);
        }

        darray_free(consecutive_bytes);
    }

    darray_free(buffer);
    return tokens;
}

darray *
lz77_decode
(
    const darray *tokens
)
{
    darray *result = darray_create(sizeof(uint8_t));

    for (size_t i = 0; i < darray_size((darray *)tokens); ++i)
    {
        lz77_token tok = darray_at((darray *)tokens, i, lz77_token);
        darray_iterator begin = darray_begin(result);
        darray_iterator_advance(begin, darray_size(result) - tok.shift, right);
        darray_iterator end = begin;
        darray_iterator_advance(end, tok.length, right);
        darray *consecutive_bytes = darray_create_iter(begin, end);

        if (tok.eom == false) {
            darray_append(consecutive_bytes, tok.letter);
        }

        for (size_t j = 0; j < darray_size(consecutive_bytes); ++j)
        {
            uint8_t curr_byte = darray_at(consecutive_bytes, j, uint8_t);
            darray_append(result, curr_byte);
        }
        darray_free(consecutive_bytes);
    }

    return result;
}