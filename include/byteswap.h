#pragma once

/* Return a value with all bytes in the 16 bit argument swapped.  */
#define bswap_16(x) (((x) >> 8) | ((x) << 8))

/* Return a value with all bytes in the 32 bit argument swapped.  */
#define bswap_32(x)                                                            \
    (((x) >> 24) | (((x)&0x00FF0000) >> 8) | (((x)&0x0000FF00) << 8) |         \
     ((x) << 24))
