#include <stdio.h>
#include <stdint.h>
#include <darray.h>
#include <utils.h>
#include <limits.h>
#include <lz77.h>
#include <math.h>
#include <memory.h>
#include <dstring.h>
#include <Huffman.h>
#include <string.h>
#include <stack.h>
#include <stdlib.h>
#include <dstring.h>
#include <lz77.h>
#include <archiver.h>
#include <bitset.h>
#include <errno.h>
#include <assert.h>

void
compress_file
(
    const char *input_filename,
    const char *output_filename
)
{
    FILE *file = fopen(input_filename, "rb");
    if (file == NULL)
    {
        perror(NULL);
        exit(1);
    }

    darray *bytes = darray_create(sizeof(uint8_t));

    int c;
    while ((c = fgetc(file)) != EOF) {
        uint8_t sym = (uint8_t)c;
        darray_append(bytes, sym);
    }

    printf("Исходный размер файла: %lu бит\n", darray_size(bytes) * 8);

    fclose(file);

    bitset *encoded = compress(bytes);

    printf("Размер закодированной последовательности: %lu бит\n", bitset_size(encoded));

    darray_free(bytes);

    FILE *file_encoded = fopen(output_filename, "wb");
    if (file_encoded == NULL)
    {
        perror(NULL);
        exit(1);
    }

    uint8_t unused_bits = bitset_size(encoded) % CHAR_BIT == 0 ? 0 : CHAR_BIT - bitset_size(encoded) % CHAR_BIT;

    fputc(unused_bits, file_encoded);

    const uint8_t *bits_data = bitset_data(encoded);

    for (size_t i = 0; i < bitset_size(encoded) / CHAR_BIT + (unused_bits == 0 ? 0 : 1); ++i)
    {
        uint8_t byte = bits_data[i];
        fputc(byte, file_encoded);
    }

    fclose(file_encoded);

    bitset_free(encoded);
}

void
bitset_append_byte
(
    bitset  *bits,
    uint8_t  byte
)
{
    for (int i = CHAR_BIT - 1; i >= 0; --i)
    {
        bool bit = (byte >> i) & 1;
        bitset_append(bits, bit);
    }
}

typedef struct bitset_impl
{
    uint8_t *data;
    size_t   size;
} bitset_impl;

void
unpack_file
(
    const char *input_filename,
    const char *output_filename
)
{
    FILE *file = fopen(input_filename, "rb");
    if (file == NULL)
    {
        perror(NULL);
        exit(1);
    }

    uint8_t unused_bits = getc(file);

    bitset *encoded_bits = bitset_create();

    int c;
    while ((c = getc(file)) != EOF) {
        uint8_t sym = (uint8_t)c;
        bitset_append_byte(encoded_bits, sym);
    }

    if (unused_bits != 0) {
        ((bitset_impl *)encoded_bits)->size -= unused_bits;
    }

    fclose(file);

    darray *decoded_bytes = unpack(encoded_bits);

    bitset_free(encoded_bits);

    FILE *file_decoded = fopen(output_filename, "w");
    if (file_decoded == NULL)
    {
        perror(NULL);
        exit(1);
    }

    for (size_t i = 0; i < darray_size(decoded_bytes); ++i)
    {
        uint8_t byte = darray_at(decoded_bytes, i, uint8_t);
        fputc(byte, file_decoded);
    }

    darray_free(decoded_bytes);

    fclose(file_decoded);
}

int main(int argc, char *argv[])
{   
    compress_file("test", "encoded.sim");
    unpack_file("encoded.sim", "decoded");

    return 0;
}

// функцию, которая сдвигает итератор и возвращает сдвинутый итератор
