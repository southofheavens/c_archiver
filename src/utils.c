#include "utils.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>

void print_bits(void *pointer, size_t type_size, size_t count)
{
    size_t i;
    for (i = 0; i < count; ++i)
    {
        int j;
        for (j = type_size - 1; j >= 0; --j)
        {
            uint8_t byte = ((char *)(pointer))[i * type_size + j];
            
            int k;
            for (k = 7; k >= 0; --k) 
            {
                printf("%d", (byte >> k) & 1);
            }
        }
        printf("\n");
    }
}
