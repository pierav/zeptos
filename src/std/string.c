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
    unsigned char c1 = '\0';
    unsigned char c2 = '\0';
    if (n >= 4) {
        size_t n4 = n >> 2;
        do {
            c1 = (unsigned char)*s1++;
            c2 = (unsigned char)*s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
            c1 = (unsigned char)*s1++;
            c2 = (unsigned char)*s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
            c1 = (unsigned char)*s1++;
            c2 = (unsigned char)*s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
            c1 = (unsigned char)*s1++;
            c2 = (unsigned char)*s2++;
            if (c1 == '\0' || c1 != c2)
                return c1 - c2;
        } while (--n4 > 0);
        n &= 3;
    }
    while (n > 0) {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0' || c1 != c2)
            return c1 - c2;
        n--;
    }
    return c1 - c2;
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

char *strncat(char *dest, const char *src, size_t n) {
    size_t dest_len = strlen(dest);
    size_t i;

    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[dest_len + i] = src[i];
    dest[dest_len + i] = '\0';

    return dest;
}

char *strcat(char *dest, const char *src) {
    size_t dest_len = strlen(dest);
    size_t i;

    for (i = 0; src[i] != '\0'; i++)
        dest[dest_len + i] = src[i];
    dest[dest_len + i] = '\0';
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';

    return dest;
}

char *strcpy(char *dest, const char *src) {
    size_t i;
    for (i = 0; src[i] != '\0'; i++)
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;
}

char *strstr(const char *string, const char *substring) {
    register char *a, *b;
    b = substring;
    if (*b == 0) {
        return string;
    }
    for (; *string != 0; string += 1) {
        if (*string != *b) {
            continue;
        }
        a = string;
        while (1) {
            if (*b == 0) {
                return string;
            }
            if (*a++ != *b++) {
                break;
            }
        }
        b = substring;
    }
    return NULL;
}

char *strtok_r(char *s, const char *delim, char **last) {
    char *spanp;
    int c, sc;
    char *tok;

    if (s == NULL && (s = *last) == NULL) {
        return NULL;
    }

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
cont:
    c = *s++;
    for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
        if (c == sc) {
            goto cont;
        }
    }

    if (c == 0) /* no non-delimiter characters */
    {
        *last = NULL;
        return NULL;
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;) {
        c = *s++;
        spanp = (char *)delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0) {
                    s = NULL;
                } else {
                    char *w = s - 1;
                    *w = '\0';
                }
                *last = s;
                return tok;
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}

char *strtok(char *s, const char *delim) {
    static char *olds;
    return strtok_r(s, delim, &olds);
}
