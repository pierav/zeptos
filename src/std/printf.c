// See LICENSE for license details.

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#define WRITEC(c)                                                              \
    do {                                                                       \
        if (out && pos < n) {                                                  \
            out[pos] = (c);                                                    \
        }                                                                      \
        pos++;                                                                 \
    } while (0);

int vsnprintf(char *out, size_t n, const char *s, va_list vl) {
    int format = 0;
    int longarg = 0;
    size_t pos = 0;
    for (; *s; s++) {
        if (format) {
            // Default value
            format = 0;
            switch (*s) {
            case 'l': {
                longarg = 1;
                format = 1;
                break;
            }
            case 'p': {
                longarg = 1;
                WRITEC('0');
                WRITEC('x');
            }
            // Fall-through
            case 'x': {
                long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
                int hexdigits = 2 * (longarg ? sizeof(long) : sizeof(int)) - 1;
                for (int i = hexdigits; i >= 0; i--) {
                    int d = (num >> (4 * i)) & 0xF;
                    WRITEC((d < 10 ? '0' + d : 'a' + d - 10));
                }
                longarg = 0;
                break;
            }
            case 'i':
            // Fall throught
            case 'u':
            // Fall throught
            case 'd': {
                long num = longarg ? va_arg(vl, long) : va_arg(vl, int);
                if (num < 0) {
                    num = -num;
                    WRITEC('-');
                }
                long digits = 1;
                for (long nn = num; nn /= 10; digits++)
                    ;
                for (int i = digits - 1; i >= 0; i--) {
                    WRITEC('0' + (num % 10));
                    num /= 10;
                }
                longarg = 0;
                break;
            }
            case 's': {
                const char *s2 = va_arg(vl, const char *);
                while (*s2) {
                    WRITEC(*s2);
                    s2++;
                }
                break;
            }
            case 'c': {
                WRITEC((char)va_arg(vl, int));
                break;
            }
            case '*': {
                int cpt = va_arg(vl, int);
                for (int i = 0; i < cpt; i++) {
                    WRITEC(' ');
                }
                format = 1;
                break;
            }
            default:
                // Ignore attributes
                format = 1;
                break;
            }
        } else if (*s == '%') {
            format = 1;
        } else {
            WRITEC(*s);
        }
    }
    if (out && pos < n) {
        out[pos] = 0;
    } else if (out && n) {
        out[n - 1] = 0;
    }
    return pos;
}
