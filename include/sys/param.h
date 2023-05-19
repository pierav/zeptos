#pragma once

#include <limits.h>
#include <stddef.h>
#include <sys/types.h>

/* BSD names for some <limits.h> values.  */
#define NBBY CHAR_BIT

/* Bit map related macros.  */
#define setbit(a, i) ((a)[(i) / NBBY] |= 1 << ((i) % NBBY))
#define clrbit(a, i) ((a)[(i) / NBBY] &= ~(1 << ((i) % NBBY)))
#define isset(a, i) ((a)[(i) / NBBY] & (1 << ((i) % NBBY)))
#define isclr(a, i) (((a)[(i) / NBBY] & (1 << ((i) % NBBY))) == 0)

/* Macros for counting and rounding.  */
#ifndef howmany
#define howmany(x, y) (((x) + ((y)-1)) / (y))
#endif
#ifdef __GNUC__
#define roundup(x, y)                                                          \
    (__builtin_constant_p(y) && powerof2(y) ? (((x) + (y)-1) & ~((y)-1))       \
                                            : ((((x) + ((y)-1)) / (y)) * (y)))
#else
#define roundup(x, y) ((((x) + ((y)-1)) / (y)) * (y))
#endif
#define powerof2(x) ((((x)-1) & (x)) == 0)

/* Macros for min/max.  */
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))