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

#if CONFIG_NOPRINTK
#define printk(fmt, ...)                                                       \
    do {                                                                       \
        ;                                                                      \
    } while (0);
#else
#define printk(fmt, ...)                                                       \
    do {                                                                       \
        printf("[%8lx/%2d] " __MODULE__ "::%s: " fmt, rdcyc(), printk_time++,  \
               __PRETTY_FUNCTION__, ##__VA_ARGS__);                            \
    } while (0);
#endif

#define panic(fmt, ...)                                                        \
    do {                                                                       \
        printk(" *** Kernel panic *** \n");                                    \
        printk(fmt, ##__VA_ARGS__);                                            \
        exit(1);                                                               \
    } while (0);
