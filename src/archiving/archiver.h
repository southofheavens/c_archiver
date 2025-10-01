#ifndef __ARCHIVER_H__
#define __ARCHIVER_H__

#include <darray.h>
#include <bitset.h>

bitset *
compress
(
    darray *bytes
);

darray *
unpack
(
    bitset *bits
);

#endif