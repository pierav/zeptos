#pragma once

#include <stdio.h>
#include <stdint.h>

#ifndef __MODULE__
#define __MODULE__ ""
#endif

static inline uint32_t rdcyc() {
  uint32_t val;
  asm volatile ("rdcycle %0":"=r" (val) ::);
  return val;
}

extern int printk_time;

#define printk(fmt, ...) do { \
  printf("[%8lx/%2d] " __MODULE__ "::%s: ", rdcyc(), printk_time++, __PRETTY_FUNCTION__); \
  printf(fmt, ##__VA_ARGS__); \
} while(0);

#define panic(fmt, ...) do { \
  printk(" *** Kernel panic *** "); \
  printk(fmt, ##__VA_ARGS__); \
  exit(1); \
} while(0);
