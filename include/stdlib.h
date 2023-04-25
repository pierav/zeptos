#pragma once

#include <stddef.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

void perror(const char *s);

__attribute__((noreturn)) void abort(void);
__attribute__((noreturn)) void exit(int status);
void *malloc(size_t size);
void free(void *ptr);
void _malloc_addblock(void *addr, size_t size);

/**
 * @brief Setup serial driver interface
 *
 * @param __getchar
 * @param __putchar
 */
void _init_serial(int (*__getchar)(void), int (*__putchar)(int));
