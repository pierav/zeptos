#pragma once
#include <stdbool.h>

#define BITS_PER_LONG 64
#define BITS_PER_LONG_LONG 64

#define BIT_MASK(nr) (1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr) ((nr) / BITS_PER_LONG)
#define BITS_PER_BYTE 8

#define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))

#define bitop(op, nr, addr)                                                    \
    ((__builtin_constant_p(nr) &&                                              \
      __builtin_constant_p((uintptr_t)(addr) != (uintptr_t)NULL) &&            \
      (uintptr_t)(addr) != (uintptr_t)NULL &&                                  \
      __builtin_constant_p(*(const unsigned long *)(addr)))                    \
         ? op(nr, addr)                                                        \
         : op(nr, addr))

static inline void ___set_bit(unsigned long nr, volatile unsigned long *addr) {
    unsigned long mask = BIT_MASK(nr);
    unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
    *p |= mask;
}

static inline void ___clear_bit(unsigned long nr,
                                volatile unsigned long *addr) {
    unsigned long mask = BIT_MASK(nr);
    unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
    *p &= ~mask;
}

static inline void ___change_bit(unsigned long nr,
                                 volatile unsigned long *addr) {
    unsigned long mask = BIT_MASK(nr);
    unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
    *p ^= mask;
}

static inline bool _test_bit(unsigned long nr,
                             const volatile unsigned long *addr) {
    return 1UL & (addr[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG - 1)));
}

static inline void bitmap_complement(unsigned long *dst,
                                     const unsigned long *src,
                                     unsigned int bits) {
    unsigned int k, lim = BITS_TO_LONGS(bits);
    for (k = 0; k < lim; ++k)
        dst[k] = ~src[k];
}

#define __set_bit(nr, addr) bitop(___set_bit, nr, addr)
#define __clear_bit(nr, addr) bitop(___clear_bit, nr, addr)
#define __change_bit(nr, addr) bitop(___change_bit, nr, addr)
#define test_bit(nr, addr) bitop(_test_bit, nr, addr)

#define DECLARE_BITMAP(name, bits) unsigned long name[BITS_TO_LONGS(bits)]
