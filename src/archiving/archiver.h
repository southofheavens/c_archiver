#ifndef __ARCHIVER_H__
#define __ARCHIVER_H__

#include <dstring.h>
#include <darray.h>
#include <bitset.h>

bitset *
compress
(
    dstring *bytes
);

dstring *
unpack
(
    bitset *bits
);

// нужно убрать из этого файла

dstring *
bitset_to_string
(
    bitset *bits
);

#endif