#pragma once

#include <stddef.h>
#include <strings.h>

// - find first bit set in a word
int ffs(int i);

int ffsl(long int i);
int ffsll(long long int i);

// - compare two strings ignoring case
int strcasecmp(const char *s1, const char *s2);
int strncasecmp(const char *s1, const char *s2, size_t n);

// int strcasecmp_l(const char *s1, const char *s2, locale_t locale);
// int strncasecmp_l(const char *s1, const char *s2, size_t n, locale_t locale);
