#pragma once

#include <stdint.h>

int clint_init(uintptr_t clint_addr, uint64_t period);
void clint_timer_it(uintptr_t cause, uintptr_t epc) __attribute__ ((interrupt));
void clint_ipi_it(uintptr_t cause, uintptr_t epc) __attribute__ ((interrupt));
void clint_mswi_write(uint64_t msip);