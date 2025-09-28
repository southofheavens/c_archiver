#ifndef __ARCHIVER_H__
#define __ARCHIVER_H__

#include <dstring.h>
#include <darray.h>

darray *
compress
(
    dstring *bytes
);

dstring *
unpack
(
    darray *bits
);

#endif