#include <stdio.h>
#include <stdint.h>
#include <darray.h>
#include <utils.h>
#include <limits.h>
// #include <Huffman.h>
// #include <string.h>
// #include <stack.h>
// #include <stdlib.h>
// #include <dstring.h>
// #include <lz77.h>
// #include <archiver.h>
// #include <bitset.h>
// #include <assert.h>

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

int main(int argc, char *argv[])
{   
    darray *haystack = darray_create(sizeof(uint8_t));
    uint8_t byte;
    byte = 'a';
    darray_append(haystack, byte);
    byte = 'b';
    darray_append(haystack, byte);
    byte = 'r';
    darray_append(haystack, byte);
    byte = 'a';
    darray_append(haystack, byte);
    byte = 'c';
    darray_append(haystack, byte);
    byte = 'a';
    darray_append(haystack, byte);
    byte = 'd';
    darray_append(haystack, byte);
    byte = 'a';
    darray_append(haystack, byte);
    byte = 'b';
    darray_append(haystack, byte);
    byte = 'r';
    darray_append(haystack, byte);
    byte = 'a';
    darray_append(haystack, byte);

    darray *needle = darray_create(sizeof(uint8_t));
    byte = 'b';
    darray_append(needle, byte);
    byte = 'e';
    darray_append(needle, byte);

    size_t pos = lz77_search_byte_seq_in_buffer(haystack, needle);

    printf("%lu\n",pos);

    return 0;
}

// проблема в строках
// необходимо будет всё переписать на darray и реализовать для darray функцию поиска последовательности
// функцию необходимо реализовать в lz77.h, а не в darray.h