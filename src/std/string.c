#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

size_t strlen(const char *s) {
    const char *p = s;
    for (;; s++) {
        if (!*s)
            break;
    }
    return s - p;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

char *strchr(const char *s, int c) {
    char *p = (char *)s;
    while (*p) {
        if (*p == c)
            return p;
        p++;
    }
    return NULL;
}

char *strrchr(register const char *s, int c) {
    char *rtnval = 0;
    do {
        if (*s == c)
            rtnval = (char *)s;
    } while (*s++);
    return (rtnval);
}

size_t strnlen(const char *s, size_t maxlen) {
    size_t len;
    for (len = 0; len < maxlen; len++, s++) {
        if (!*s)
            break;
    }
    return (len);
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- > 0 && *s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

char *strncpy(char *dst, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i]; i++) {
        dst[i] = src[i];
    }
    for (; i < n; i++) {
        dst[i] = 0;
    }
    return dst;
}

char *strdup(const char *s) {
    char *copy;
    size_t len = strlen(s) + 1;
    if (!(copy = malloc(len)))
        return (NULL);
    memcpy(copy, s, len);
    return copy;
}

void *memchr(const void *s, int c, size_t n) {
    unsigned char *p = (unsigned char *)s;
    while (n-- > 0) {
        if (*p == c)
            return p;
        p++;
    }
    return NULL;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    while (n-- > 0) {
        char c1 = *(const char *)s1++;
        char c2 = *(const char *)s2++;
        if (c1 != c2) {
            return c1 - c2;
        }
    }
    return 0;
}

void *memcpy(void *dest, const void *src, size_t n) {
    char *p = dest;
    while (n-- > 0) {
        *(char *)dest++ = *(char *)src++;
    }
    return p;
}

void *memmove(void *dest, const void *src, size_t n) {
    unsigned char *from = (unsigned char *)src;
    unsigned char *to = (unsigned char *)dest;

    if (from == to || n == 0)
        return dest;
    if (to > from && to - from < (int)n) {
        int i;
        for (i = n - 1; i >= 0; i--)
            to[i] = from[i];
        return dest;
    }
    if (from > to && from - to < (int)n) {
        size_t i;
        for (i = 0; i < n; i++)
            to[i] = from[i];
        return dest;
    }
    memcpy(dest, src, n);
    return dest;
}

void *memset(void *dest, int c, size_t n) {
    char *p = dest;
    while (n-- > 0) {
        *(char *)dest++ = c;
    }
    return p;
}

char *strerror(int errnum) {
    (void)errnum;
    return "*strerror*";
}