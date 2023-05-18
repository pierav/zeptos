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

// int printf(const char *format, ...) {
//     pthread_mutex_lock(&_print_mutex);
//     va_list vl;
//     va_start(vl, format);
//     int res = vprintf(format, vl);
//     va_end(vl);
//     pthread_mutex_unlock(&_print_mutex);
//     return res;
// }

// int vprintf(const char *s, va_list vl) {
//     char *out;
//     int res = vsnprintf(NULL, 0, s, vl);
//     out = alloca(res + 1);
//     vsnprintf(out, res + 1, s, vl);
//     while (*out)
//         putchar(*out++);
//     return res;
// }
