#ifndef __DSTRING_H__
#define __DSTRING_H__

#include <stdio.h>
#include <limits.h>

#define DSTRING_NPOS SIZE_T_MAX

typedef struct dstring dstring;

/**
 * Возвращает пустую строку
 */
dstring *
dstring_create_empty
(
    void
);

/**
 * Возвращает строку с длиной LENGTH
 */
dstring *
dstring_create_length
(
    size_t length
);

/**
 * Возвращает строку из c-style строки STR
 */
dstring *
dstring_create_c_str
(
    const char *str
);

/**
 * Возвращает строку длины COUNT, состоящую из букв LETTER
 */
dstring *
dstring_create_letters
(
    const char letter, 
    size_t     count
);

/**
 * Возвращает подстроку от строки SRC начиная с позиции BEGIN и длиной LENGTH
 */
dstring *
dstring_substr
(
    const dstring *src,
    size_t         begin,
    size_t         length
);

/**
 * Возвращает копию строки DSTR
 */
dstring *
dstring_create_dstring
(
    const dstring *dstr
);

/**
 * Очищает ресурсы, занятые строкой DSTR
 */
void
dstring_free
(
    dstring *dstr
);

/**
 * Возвращает длину строки DSTR
 */
size_t
dstring_length
(
    dstring *dstr
);

/**
 * Возвращает c-style строку строки DSTR
 */
const char *
dstring_get_c_str
(
    dstring *dstr
);

/**
 * Производит конкатенацию строки DEST со строкой SRC
 */
void
dstring_concat
(
    dstring       *dest,
    const dstring *src
);

/**
 * Производит конкатенацию строки DSTR с символом LETTER
 */
void 
dstring_append
(
    dstring *dstr,
    char     letter
);

/**
 * Возвращает результат сравнения строк DSTR1 и DSTR2:
 * - result < 0: dstr1 < dstr2
 * - result == 0: dstr1 == dstr2
 * - result > 0: dstr1 > dstr
 */
int
dstring_compare
(
    dstring *dstr1,
    dstring *dstr2
);

/**
 * Возвращает индекс нахождения подстроки NEEDLE в строке HAYSTACK.
 * Поиск проводится слево направо. Если подстрока не найдена,
 * result == DSTRING_NPOS
 */
size_t
dstring_find 
(
    dstring *haystack,
    dstring *needle
);

/**
 * Возвращает индекс нахождения подстроки NEEDLE в строке HAYSTACK.
 * Поиск проводится справа налево. Если подстрока не найдена,
 * result == DSTRING_NPOS
 */
size_t
dstring_find_r
(
    dstring *haystack,
    dstring *needle
);

/**
 * Возвращает символ строки DSTR, расположенный по индексу INDEX
 */
char
dstring_at
(
    dstring *dstr,
    size_t   index
);

#endif