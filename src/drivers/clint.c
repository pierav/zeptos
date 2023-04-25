#include <stdint.h>
#include <stdio.h>

static volatile uint64_t * mtime;
static volatile uint64_t * mtimecmp;
static uint64_t timer_period;

void clint_timer_it(uintptr_t cause, uintptr_t epc) {
    (void)(cause);
    (void)(epc);
    printf("MINT\n");
    *mtimecmp = *mtime + timer_period;
    // printf("MINT END\n");
}

void clint_ipi_it(uintptr_t cause, uintptr_t epc) {
    (void)(cause);
    (void)(epc);
    printf("clint_ipi_it TODO");
}

void printf_status(uint64_t mstatus, uint64_t mie, uint64_t mip, uint64_t mcause) {
    asm volatile ("csrr %[reg], mie" : [reg] "=r" (mie));
    asm volatile ("csrr %[reg], mip" : [reg] "=r" (mip));
    asm volatile ("csrr %[reg], mstatus" : [reg] "=r" (mstatus));
    asm volatile ("csrr %[reg], mcause" : [reg] "=r" (mcause));
    printf("mie=%x, mip=%x, mstatus=%x, mcause=%x\r\n", mie, mip, mstatus, mcause);
}

int clint_init(uintptr_t clint_addr, uint64_t period) {
    mtime = (uint64_t*)(clint_addr + 0xbff8);
    mtimecmp = (uint64_t*)(clint_addr + 0x4000);
    timer_period = period;
    // printf("mtimecmp=%x\n, mtime=%x\n", mtimecmp, mtime);
    // *mtimecmp = *mtime + mtime_INTERRUPT_PERIOD;
    return 0;
}
