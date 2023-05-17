#pragma once

#include <stddef.h>

void *memchr(const void *s, int c, size_t n);
int memcmp(const void *, const void *, size_t);
void *memcpy(void *, const void *, size_t);
void *memset(void *, int, size_t);
void *memmove(void *dest, const void *src, size_t n);

char *strchr(const char *s, int c);
char *strrchr(register const char *s, int c);

int strcmp(const char *, const char *);
size_t strlen(const char *);
int strncmp(const char *, const char *, size_t);
size_t strnlen(const char *s, size_t maxlen);
char *strdup(const char *s);

char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);

char *strerror(int errnum);

// - locate a substring
char *strstr(const char *haystack, const char *needle);

// - extract tokens from strings
char *strtok(char *str, const char *delim);
char *strtok_r(char *str, const char *delim, char **saveptr);
