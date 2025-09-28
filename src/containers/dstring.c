#include <dstring.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define DSTRING_INIT_CAPACITY 16

typedef struct dstring
{ 
    char   *str;
    size_t  length;
    size_t  capacity;
} dstring;

dstring *
dstring_create_empty
(
    void
)
{
    dstring *dstr = (dstring *)malloc(sizeof(dstring));
    if (dstr == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    dstr->length = 0;
    dstr->capacity = DSTRING_INIT_CAPACITY;

    dstr->str = (char *)malloc(dstr->capacity);
    if (dstr->str == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    dstr->str[dstr->length] = '\0';

    return dstr;
}

dstring *
dstring_create_length
(
    size_t length
)
{
    dstring *dstr = (dstring *)malloc(sizeof(dstring));
    if (dstr == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    dstr->length = length;
    dstr->capacity = 
        (dstr->length >= DSTRING_INIT_CAPACITY ? dstr->length + sizeof(char) : DSTRING_INIT_CAPACITY);

    dstr->str = (char *)malloc(dstr->capacity);
    if (dstr->str == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }   

    dstr->str[dstr->length] = '\0';

    return dstr;
}

dstring *
dstring_create_c_str
(
    const char *str
)
{
    dstring *dstr = (dstring *)malloc(sizeof(dstring));
    if (dstr == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    dstr->length = strlen(str);
    dstr->capacity = 
        (dstr->length >= DSTRING_INIT_CAPACITY ? dstr->length + sizeof(char) : DSTRING_INIT_CAPACITY);

    dstr->str = (char *)malloc(dstr->capacity);
    if (dstr->str == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    strcpy(dstr->str, str);

    return dstr;
}

dstring *
dstring_create_letters
(
    const char letter, 
    size_t     count
)
{
    dstring *dstr = (dstring *)malloc(sizeof(dstring));
    if (dstr == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    dstr->length = count;
    dstr->capacity = 
        (dstr->length >= DSTRING_INIT_CAPACITY ? dstr->length + sizeof(char) : DSTRING_INIT_CAPACITY);

    dstr->str = (char *)malloc(dstr->capacity);
    if (dstr->str == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    memset(dstr->str, letter, count);
    dstr->str[dstr->length] = '\0';

    return dstr;
}

dstring *
dstring_substr
(
    const dstring *src,
    size_t         begin,
    size_t         length
)
{    
    dstring *dstr = (dstring *)malloc(sizeof(dstring));
    if (dstr == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    dstr->length =  
        src->length > begin + length ? length : (src->length > begin ? src->length - begin : 0);
    dstr->capacity = 
        (dstr->length >= DSTRING_INIT_CAPACITY ? dstr->length + sizeof(char) : DSTRING_INIT_CAPACITY);
    
    dstr->str = (char *)malloc(dstr->length + sizeof(char));
    if (dstr->str == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    strncpy(dstr->str, (&(src->str)[begin]), dstr->length);

    return dstr;
}

dstring *
dstring_create_dstring
(
    const dstring *dstr
)
{
    dstring *copy = (dstring *)malloc(sizeof(dstring));
    if (dstr == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    copy->length = dstr->length;
    copy->capacity = dstr->capacity;

    copy->str = (char *)malloc(copy->capacity);
    if (copy->str == NULL)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }

    strcpy(copy->str, dstr->str);

    return copy;
}

void
dstring_free
(
    dstring *dstr
)
{
    free(dstr->str);
    free(dstr);
}

size_t
dstring_length
(
    dstring *dstr
)
{
    return dstr->length;
}

const char *
dstring_get_c_str
(
    dstring *dstr
)
{
    return dstr->str;
}

void
dstring_concat
(
    dstring       *dest,
    const dstring *src
)
{
    if (dest->capacity - dest->length <= src->length)
    {
        dest->capacity = dest->length + src->length + sizeof(char);
        dest->str = (char *)realloc(dest->str, dest->capacity);
        if (dest->str == NULL)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
    }

    strcat(dest->str, src->str);
    dest->length += src->length;
}

void 
dstring_append
(
    dstring *dstr,
    char     letter
)
{
    if (dstr->length == dstr->capacity - 1)
    {
        dstr->capacity *= 2;
        dstr->str = (char *)realloc(dstr->str, dstr->capacity);
        if (dstr->str == NULL)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(1);
        }
    }

    (dstr->str)[(dstr->length)++] = letter;
    (dstr->str)[dstr->length] = '\0';
}

int
dstring_compare
(
    dstring *dstr1,
    dstring *dstr2
)
{
    return strcmp(dstr1->str, dstr2->str);
}

size_t
dstring_find 
(
    dstring *haystack,
    dstring *needle
)
{
    char *result = strstr(haystack->str, needle->str);
    return (result ? result - haystack->str : DSTRING_NPOS);
}

size_t
dstring_find_r
(
    dstring *haystack,
    dstring *needle
)
{
    if (haystack->length < needle->length) {
        return DSTRING_NPOS;
    }

    size_t count_of_comparison, pos, i;
    count_of_comparison = haystack->length - needle->length + 1;
    pos = haystack->length - needle->length;
    i = 0;

    while (i < count_of_comparison)
    {
        if (strncmp(haystack->str + pos, needle->str, needle->length) == 0) {
            return pos;
        }
        ++i, --pos;
    }
    return DSTRING_NPOS;
}

char
dstring_at
(
    dstring *dstr,
    size_t   index
)
{
    return (dstr->str)[index];
}