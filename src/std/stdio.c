#include <alloca.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

void perror(const char *s) {
    printf("perror [%d]: ", errno);
    puts(s);
}

int puts(const char *s) {
    while (*s)
        putchar(*s++);
    putchar('\n');
    return 0;
}

int printf(const char *s, ...) {
    int res = 0;
    va_list vl;
    va_start(vl, s);
    res = vprintf(s, vl);
    va_end(vl);
    return res;
}

int snprintf(char *out, size_t n, const char *s, ...) {
    va_list vl;
    va_start(vl, s);
    int res = vsnprintf(out, n, s, vl);
    va_end(vl);
    return res;
}

int vprintf(const char *s, va_list vl) {
    char *out;
    int res = vsnprintf(NULL, -1, s, vl);
    out = alloca(res + 1);
    vsnprintf(out, res + 1, s, vl);
    while (*out)
        putchar(*out++);
    return res;
}
