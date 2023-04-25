#include "poweroff.h"
#include <stdio.h>
#include <stdlib.h>

void abort(void) {
    puts("abort");
    exit(-1);
}

void exit(int status) {
    tohost_exit(status);
    __builtin_unreachable();
}

int (*_getchar)(void);
int (*_putchar)(int);

void _init_serial(int (*__getchar)(void), int (*__putchar)(int)) {
    _getchar = __getchar;
    _putchar = __putchar;
}

int getchar() { return _getchar(); }

int putchar(int ch) { return _putchar(ch); }
