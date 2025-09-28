#include <lz77.h>
#include <memory.h>

// #define BUFFER_SIZE 2048

lz77_token lz77_search_for_occurrence
(
    const dstring *buffer,
    const dstring *str,
    size_t        *index
)
{
    lz77_token token;
    memset(&token, 0, sizeof(token));

    // Индекс начала текущей последовательности
    //size_t start_index = dstring_length((dstring *)buffer) < BUFFER_SIZE ? *index : BUFFER_SIZE + 1;
    size_t start_index = *index;
    // Текущая последовательность байт
    dstring *curr_substr = dstring_create_empty();
    // Индекс нахождения curr_substr в буфере
    size_t index_of_occurrence;

    size_t i = 0;
    do    
    {
        char curr_char = dstring_at((dstring *)str, *index);
        dstring_append(curr_substr, curr_char);

        size_t index_of_curr_occur;
        if ((index_of_curr_occur = 
            dstring_find_r((dstring *)buffer, curr_substr)) != DSTRING_NPOS)
        {
            index_of_occurrence = index_of_curr_occur;

            token.shift = start_index - index_of_occurrence;
            token.length = dstring_length(curr_substr);

            if (*index == dstring_length((dstring *)str) - 1) {
                token.eom = true;
            }
            else if (i == dstring_length((dstring *)buffer) - 1) {
                token.letter = dstring_at((dstring *)str, ++(*index));
            }
        }
        else 
        {
            token.letter = curr_char;
            (*index)++;
            break;
        }

        (*index)++;
        i++;
    }
    while (i < dstring_length((dstring *)buffer) 
        && *index < dstring_length((dstring *)str));

    dstring_free(curr_substr);

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

        printf("%hu - %hu - %c - %d\n", another_token.shift, another_token.length, another_token.letter, another_token.eom);

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