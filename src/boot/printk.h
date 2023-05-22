#pragma once

#include "config.h"
#include <stdint.h>
#include <stdio.h>

#ifndef __MODULE__
#define __MODULE__ ""
#endif

static inline uint32_t rdcyc() {
    uint32_t val;
    asm volatile("rdcycle %0" : "=r"(val)::);
    return val;
}

extern int printk_time;

#define _printk_internal(fmt, ...)                                             \
    do {                                                                       \
        uint64_t mhartid;                                                      \
        asm volatile("csrr %[reg], mhartid" : [reg] "=r"(mhartid));            \
        printf("[%8ld][%01d] " __MODULE__ "::%s: " fmt, rdcyc(), mhartid,      \
               __PRETTY_FUNCTION__, ##__VA_ARGS__);                            \
    } while (0)

#ifdef CONFIG_NOPRINTK
#define printk(fmt, ...)                                                       \
    do {                                                                       \
        ;                                                                      \
    } while (0);
#else
#define printk(fmt, ...) _printk_internal(fmt, ##__VA_ARGS__)
#endif

#define panic(fmt, ...)                                                        \
    do {                                                                       \
        _printk_internal(" *** Kernel panic *** \n");                          \
        _printk_internal(fmt, ##__VA_ARGS__);                                  \
        exit(1);                                                               \
    } while (0)
