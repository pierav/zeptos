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
    printk("MINT\n");
    *mtimecmp = *mtime + timer_period;
    // printf("MINT END\n");
}

void clint_ipi_it(uintptr_t cause, uintptr_t epc) {
    (void)(cause);
    (void)(epc);
    uint64_t mhartid;
    asm volatile("csrr %[reg], mhartid" : [reg] "=r"(mhartid));
    printk("clint_ipi_it %d TODO\n", mhartid);
    clint_mswi_unset(mhartid);
}

void printf_status(uint64_t mstatus, uint64_t mie, uint64_t mip,
                   uint64_t mcause) {
    asm volatile("csrr %[reg], mie" : [reg] "=r"(mie));
    asm volatile("csrr %[reg], mip" : [reg] "=r"(mip));
    asm volatile("csrr %[reg], mstatus" : [reg] "=r"(mstatus));
    asm volatile("csrr %[reg], mcause" : [reg] "=r"(mcause));
    printf("mie=%x, mip=%x, mstatus=%x, mcause=%x\r\n", mie, mip, mstatus,
           mcause);
}

int clint_init(uintptr_t clint_addr, uint64_t period) {
    mswi = (uint32_t *)(clint_addr + 0x0);
    mtimecmp = (uint64_t *)(clint_addr + 0x4000);
    mtime = (uint64_t *)(clint_addr + 0xbff8);
    timer_period = period;
    // printf("mtimecmp=%x\n, mtime=%x\n", mtimecmp, mtime);
    // *mtimecmp = *mtime + mtime_INTERRUPT_PERIOD;
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