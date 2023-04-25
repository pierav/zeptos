#pragma once

#include <stddef.h>

void *memchr(const void *s, int c, size_t n);
int memcmp(const void *, const void *, size_t);
void *memcpy(void *, const void *, size_t);
void *memset(void *, int, size_t);
void *memmove(void *dest, const void *src, size_t n);

char *strchr(const char *s, int c);
char *strrchr(register const char *s, int c);
unsigned long strtoul(const char *nptr, char **endptr, register int base);

int strcmp(const char *, const char *);
size_t strlen(const char *);
int strncmp(const char *, const char *, size_t);
char *strncpy(char *, const char *, size_t);
size_t strnlen(const char *s, size_t maxlen);
char *strdup(const char *s);
