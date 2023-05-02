#include "poweroff.h"
#include <stdint.h>

// TODO: fdt parsing

// 0: HTIF
extern volatile uint64_t tohost;
extern volatile uint64_t fromhost;

// 1: Syfive test
static volatile uint32_t *syfive_test = (void *)0x100000;
#define SIFIVE_TEST_FAIL 0x3333
#define SIFIVE_TEST_PASS 0x5555

// 2: pass / fail symbols
#define __GOOD_BAD_TRAP_FUNC __attribute__((noreturn, noinline))

void __GOOD_BAD_TRAP_FUNC pass() {
    tohost = 1; // FESVR exit OK
    *syfive_test = SIFIVE_TEST_PASS;
    while (1)
        ;
    __builtin_unreachable();
}

void __GOOD_BAD_TRAP_FUNC fail(uintptr_t code) {
    tohost = (code << 1) | 1; // FESVR exit
    *syfive_test = SIFIVE_TEST_FAIL;
    while (1)
        ;
    __builtin_unreachable();
}

void __attribute__((noreturn, noinline)) tohost_exit(uintptr_t code) {
    if (code) { // GOOD TRAP / BAD TRAP exit
        fail(code);
    } else {
        pass();
    }
    __builtin_unreachable();
}
