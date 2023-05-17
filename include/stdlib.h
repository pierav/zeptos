#pragma once

#include <stddef.h>
#include <stdint.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

__attribute__((noreturn)) void abort(void);
__attribute__((noreturn)) void exit(int status);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);

void free(void *ptr);
void _malloc_addblock(void *addr, size_t size);

/**
 * @brief Setup serial driver interface
 *
 * @param __getchar
 * @param __putchar
 */
void _init_serial(int (*__getchar)(void), int (*__putchar)(int));

int atoi(const char *nptr);
long atol(const char *nptr);
long long atoll(const char *nptr);

unsigned long strtoul(const char *nptr, char **endptr, register int base);
long strtol(const char *nptr, char **endptr, int base);

long double strtold(const char *str, char **endptr);
double strtod(const char *str, char **endptr);
float strtof(const char *str, char **endptr);

double atof(const char *str);

void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));

int rand();
void srand(uint64_t seed);

int abs(int j);
long int labs(long int j);
long long int llabs(long long int j);

#include <stdlib.h>

char *getenv(const char *name);
char *secure_getenv(const char *name);
