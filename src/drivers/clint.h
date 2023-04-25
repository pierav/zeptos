#pragma once

#include <stdint.h>

int clint_init(uintptr_t clint_addr, uint64_t period);

void clint_timer_it(uintptr_t cause, uintptr_t epc);
void clint_ipi_it(uintptr_t cause, uintptr_t epc);

void clint_mswi_set(uint64_t msip);
void clint_mswi_unset(uint64_t msip);