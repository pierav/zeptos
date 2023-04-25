#include <stdint.h>
#include "poweroff.h"

extern volatile uint64_t tohost;
extern volatile uint64_t fromhost;

void __attribute__((noreturn, noinline)) pass () {
    tohost = 1; // FESVR exit OK
    while(1);
    __builtin_unreachable();
}

void __attribute__((noreturn, noinline)) fail (uintptr_t code) {
    tohost = (code << 1) | 1; // FESVR exit
    while(1);
    __builtin_unreachable();
}

void __attribute__((noreturn, noinline)) tohost_exit(uintptr_t code)
{
    if (code) { // GOOD TRAP / BAD TRAP exit
        fail(code);
    } else {
        pass();
    }
    __builtin_unreachable();
}
