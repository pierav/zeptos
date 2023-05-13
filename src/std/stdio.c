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

#include <pthread.h>
pthread_mutex_t _print_mutex = PTHREAD_MUTEX_INITIALIZER;

int printf(const char *format, ...) {
    pthread_mutex_lock(&_print_mutex);
    va_list vl;
    va_start(vl, format);
    int res = vprintf(format, vl);
    va_end(vl);
    pthread_mutex_unlock(&_print_mutex);
    return res;
}

// int sprintf(char *str, const char *format, ...){
//     va_list vl;
//     va_start(vl, format);
//     int res = vsprintf(str, format, vl);
//     va_end(vl);
//     return res;
// }

// int snprintf(char *out, size_t n, const char *format, ...) {
//     va_list vl;
//     va_start(vl, format);
//     int res = vsnprintf(out, n, format, vl);
//     va_end(vl);
//     return res;
// }

int vprintf(const char *s, va_list vl) {
    char *out;
    int res = vsnprintf(NULL, 0, s, vl);
    out = alloca(res + 1);
    vsnprintf(out, res + 1, s, vl);
    while (*out)
        putchar(*out++);
    return res;
}

// int vsprintf(char *str, const char *format, va_list ap){
//     return vsnprintf(str, -1, format, ap);
// }

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
