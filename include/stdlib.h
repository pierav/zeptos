#pragma once

#include <stddef.h>
#include <stdint.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

void perror(const char *s);

__attribute__((noreturn)) void abort(void);
__attribute__((noreturn)) void exit(int status);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void _malloc_addblock(void *addr, size_t size);

/**
 * @brief Setup serial driver interface
 *
 * @param __getchar
 * @param __putchar
 */
void _init_serial(int (*__getchar)(void), int (*__putchar)(int));

unsigned long strtoul(const char *nptr, char **endptr, register int base);
long strtol(const char *nptr, char **endptr, int base);

long double strtold(const char *str, char **endptr);
double strtod(const char *str, char **endptr);
float strtof(const char *str, char **endptr);

void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));

int rand();
void srand(uint64_t seed);
