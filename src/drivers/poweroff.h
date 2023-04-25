#pragma once

#include <stdint.h>

void __attribute__((noreturn, noinline)) tohost_exit(uintptr_t code);