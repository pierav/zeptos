#include "printk.h"
#include <alloca.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

void perror(const char *s) { printk("perror [%d]: %s\n", errno, s); }

int puts(const char *s) {
    while (*s)
        putchar(*s++);
    putchar('\n');
    return 0;
}

#include "lock.h"
static atomic_t printf_lock = {.counter = 0};

int printf(const char *s, ...) {
    get_lock(&printf_lock);
    int res = 0;
    va_list vl;
    va_start(vl, s);
    res = vprintf(s, vl);
    va_end(vl);
    put_lock(&printf_lock);
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

// Fake filesystem

struct FILE {
    uint64_t id;
    char *filename;
    uint64_t size;
    void *base;
};

FILE *stdout = NULL;
FILE *stderr = NULL;
FILE *stdin = NULL;

int fprintf(FILE *stream, const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    vprintf(format, vl);
    va_end(vl);
    return 0;
}

int fputc(int c, FILE *stream) { return putchar(c); }

int putc(int c, FILE *stream) { return putchar(c); }

int vfprintf(FILE *stream, const char *format, va_list ap) {
    return vprintf(format, ap);
}

FILE *fopen(const char *filename, const char *mode) { return NULL; }
int fclose(FILE *stream) { return 0; }
int fflush(FILE *stream) { return 0; }

int fgetc(FILE *stream) { return EOF; }
