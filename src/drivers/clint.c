#include "clint.h"
#include <assert.h>
#include <printk.h>
#include <stdint.h>
#include <stdio.h>

// https://github.com/riscv/riscv-aclint/blob/main/riscv-aclint.adoc

static volatile uint64_t *mtime;
static volatile uint64_t *mtimecmp;
static volatile uint32_t *mswi;

static uint64_t timer_period;

void clint_timer_it(uintptr_t cause, uintptr_t epc) {
    (void)(cause);
    (void)(epc);
    printk("IT\n");
    *mtimecmp = *mtime + timer_period;
}

void clint_ipi_it(uintptr_t cause, uintptr_t epc) {
    (void)(cause);
    (void)(epc);
    uint64_t mhartid;
    asm volatile("csrr %[reg], mhartid" : [reg] "=r"(mhartid));
    printk("%d TODO\n", mhartid);
    clint_mswi_unset(mhartid);
}

int clint_init(uintptr_t clint_addr, uint64_t period) {
    mswi = (uint32_t *)(clint_addr + 0x0);
    mtimecmp = (uint64_t *)(clint_addr + 0x4000);
    mtime = (uint64_t *)(clint_addr + 0xbff8);
    timer_period = period;
    return 0;
}

void clint_mswi_set(uint64_t msip) {
    assert(msip < 4096);
    *(mswi + msip) = 1;
}

void clint_mswi_unset(uint64_t msip) {
    assert(msip < 4096);
    *(mswi + msip) = 0;
}