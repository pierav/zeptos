#include "banner.h"
#include "drivers/clint.h"
#include "drivers/ns16550a.h"
#include "dts.h"
#include "encoding.h"
#include "fdt.h"
#include "libfdt.h"
#include "libfdt_env.h"
#include "printk.h"
#include <byteswap.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MCAUSE_INT 63
#define MCAUSE_MISALIGNED_FETCH (((uint64_t)0 << MCAUSE_INT) | 0)
#define MCAUSE_FAULT_FETCH (((uint64_t)0 << MCAUSE_INT) | 1)
#define MCAUSE_ILLEGAL_INSTRUCTION (((uint64_t)0 << MCAUSE_INT) | 2)
#define MCAUSE_BREAKPOINT (((uint64_t)0 << MCAUSE_INT) | 3)
#define MCAUSE_MISALIGNED_LOAD (((uint64_t)0 << MCAUSE_INT) | 4)
#define MCAUSE_FAULT_LOAD (((uint64_t)0 << MCAUSE_INT) | 5)
#define MCAUSE_MISALIGNED_STORE (((uint64_t)0 << MCAUSE_INT) | 6)
#define MCAUSE_FAULT_STORE (((uint64_t)0 << MCAUSE_INT) | 7)
#define MCAUSE_ECALL_U (((uint64_t)0 << MCAUSE_INT) | 8)
#define MCAUSE_ECALL_S (((uint64_t)0 << MCAUSE_INT) | 9)
#define MCAUSE_ECALL_M (((uint64_t)0 << MCAUSE_INT) | 11)
#define MCAUSE_PAGE_FAULT_INST (((uint64_t)0 << MCAUSE_INT) | 12)
#define MCAUSE_PAGE_FAULT_LOAD (((uint64_t)0 << MCAUSE_INT) | 13)
#define MCAUSE_PAGE_FAULT_STORE (((uint64_t)0 << MCAUSE_INT) | 15)
#define MCAUSE_INTERRUPT (((uint64_t)1 << MCAUSE_INT))

const char *DISPLAY_MCAUSE[] = {[0] = "MISALIGNED_FETCH",
                                [1] = "FAULT_FETCH",
                                [2] = "ILLEGAL_INSTRUCTION",
                                [3] = "BREAKPOINT",
                                [4] = "MISALIGNED_LOAD",
                                [5] = "FAULT_LOAD",
                                [6] = "MISALIGNED_STORE",
                                [7] = "FAULT_STORE",
                                [8] = "ECALL_U",
                                [9] = "ECALL_S",
                                [11] = "ECALL_M",
                                [12] = "PAGE_FAULT_INST",
                                [13] = "PAGE_FAULT_LOAD",
                                [15] = "PAGE_FAULT_STORE"};

#define NB_EXCEPT 16
#define NB_IT 64

void (*handle_it_arr[NB_IT])(uintptr_t, uintptr_t) = {[3] = clint_ipi_it,
                                                      [7] = clint_timer_it};

void (*handle_except_arr[NB_EXCEPT])(uintptr_t, uintptr_t) = {NULL};

uintptr_t handle_trap(uintptr_t cause, uintptr_t epc, uintptr_t regs[32]) {
    void (*handle)(uintptr_t, uintptr_t) = NULL;
    char isit = cause >= MCAUSE_INTERRUPT;
    if (isit) { // Interrupt
        uint64_t it = (cause - MCAUSE_INTERRUPT);
        handle = handle_it_arr[it];
    } else { // Exception
        handle = handle_except_arr[cause];
    }
    if (handle) {
        handle(cause, epc);
    } else {
        uintptr_t mtval;
        asm volatile("csrr %[reg], mtval" : [reg] "=r"(mtval));
        printf("Bad trap %s: %x epc=%lx tval=%lx\n",
               isit ? "IT" : DISPLAY_MCAUSE[cause], cause, epc, mtval);
        exit(1000 + cause);
    }
    (void)regs;
    return epc;
}

__attribute__((weak)) char *_argv[] = {"zeptos", "hello world"};
__attribute__((weak)) int _argc = 2;

int __attribute__((weak)) main(int argc, char **argv) {
    if (argc == 2) {
        printf("%s >>> %s\n", argv[0], argv[1]);
    }
    printf("Implement main()!\n");
    return -1;
}

// 50 MHz
// uart_bus #(.BAUD_RATE(115200), .PARITY_EN(0)) i_uart_bus (.rx(tx), .tx(rx),
// .rx_en(1'b1));
#define ARIANE_UART_ADDR 0x10000000
#define ARIANE_UART_FREQ 50000000
#define ARIANE_UART_BAUDRATE 115200
#define ARIANE_UART_REG_SHIFT 2
#define ARIANE_UART_REG_WIDTH 4

#define CLINT_INTERRUPT_PERIOD 1200000000

void park() {
    while (1)
        ;
}

extern char _end;
extern char _bss_start;
extern char _bss_end;

void _trap(int i) {
    while (1) {
        i = i * 1;
        asm volatile("wfi");
    }
}

void _init(uint64_t cid, uint64_t dtb) {
    int tos;
    // Only core 0 boots
    if (cid != 0) {
        uint32_t mie = MIP_MSIP; // Only enable Soft INT
        asm volatile("csrw mie, %[reg]" : : [reg] "r"((uint32_t)mie));
        asm volatile("csrsi mstatus, 8"); // MSTATUS_MIE
        park();
    }

    // Check if dtb is valid
    if (fdt_magic((void *)dtb) != FDT_MAGIC) {
        // TODO: post console printk("Invalid FDT");
        exit(1);
    }

#ifdef CONFIG_DTS_STATIC
    uint64_t _fdt = 0xCafeCafe;
    void *fdt = &_fdt;
#else
    // Copy FDT in memory for faster $ access
    size_t fdt_size = fdt_totalsize((void *)dtb);
    char fdt[fdt_size]; // Allocate FDT on stack
    memcpy(fdt, (void *)dtb, fdt_size);
#endif

    // The first step is to initialise the Serial device
    uintptr_t ns16550_addr;
    uint32_t reg_shift, reg_io_width;
    const char *ns16550_compatible = "ns16550a";
    int res = fdt_parse_ns16550(fdt, &ns16550_addr, &reg_shift, &reg_io_width,
                                ns16550_compatible);
    if (res == 0) {
        uart8250_init((void *)ns16550_addr, ARIANE_UART_FREQ,
                      ARIANE_UART_BAUDRATE, reg_shift, reg_io_width);
        _init_serial(uart8250_getc, uart8250_putc);
    } else {
        exit(1);
    }

    // Print banner
    PRINT_BANNER();
    printk("Boot core [%d] / DTB=(0x%x)=0x%x\n", cid, fdt,
           bswap_32(*(uint32_t *)fdt));
    printk("%x.uart %s driver %2x/%2x\n", ns16550_addr, ns16550_compatible,
           reg_shift, reg_io_width);

    // Init memory
    printk("TOS = %x\n", &tos);
    printk("Init BSS [%x: %x]...\n", &_bss_start, &_bss_end);
    // memset(&_bss_start, 0, &_bss_end - &_bss_start);
    printk("Setup dyn memory...\n");
    // TODO Dts &_end + stacks...
    _malloc_addblock((void *)0x88000000, 16 * 128 * 1024); // Lest add 128 kB

    // Parse DTS
    // printk("fdt_version(p)=%d\n", fdt_version(fdt));
    // printk("fdt_last_comp_version(p)=%d\n", fdt_last_comp_version(fdt));
    // printk("fdt_totalsize(p)=%d\n", fdt_totalsize(fdt));
    // printk("fdt_off_dt_struct(p)=%d\n", fdt_off_dt_struct(fdt));
    // printk("fdt_off_dt_strings(p)=%d\n", fdt_off_dt_strings(fdt));

    // Get all CPUS
    int nr_cpus;
    const char *isa;
    const char *mmu;
    const char *compatible;
    if (fdt_parse_cpus(fdt, &nr_cpus, &compatible, &isa, &mmu) != 0) {
        panic("Invalid CPU found in DTS\n");
    }
    printk("Catch %d cpu [%s, %s, %s]\n", nr_cpus, compatible, isa, mmu);
    if (nr_cpus > CONFIG_NR_CPUS) {
        printk("WARN: %d > CONFIG_NR_CPUS\n", nr_cpus);
        nr_cpus = CONFIG_NR_CPUS;
    }
    printk("Initialise pthread with %d cpus\n", nr_cpus);
    _pthread_init(nr_cpus);

    // Plic initialisation
    // TODO

    // Clint initialisation
    uintptr_t clint_addr;
    char *clint_compatible = "riscv,clint0";
    if (fdt_parse_clint(fdt, &clint_addr, clint_compatible) != 0) {
        panic("No clint found in DTS\n");
    }
    printk("%x.%s Initialise\n", clint_addr, clint_compatible);
    clint_init(clint_addr, CLINT_INTERRUPT_PERIOD);
    printk("%x.%s register IT %x with clint_timer_it\n", clint_addr,
           clint_compatible, 7);
    printk("%x.%s register IT %x with clint_ipi_it\n", clint_addr,
           clint_compatible, 3);
    // handle_it_arr[7] = clint_timer_it;
    // handle_it_arr[3] = clint_ipi_it;
    // register IT TODO FDT id

    // machine interrupt enable
    printk("Enable ITs\n");
    // IRQ_M_SOFT, IRQ_M_TIMER, IRQ_M_EXT
    uint32_t mie = MIP_MSIP | MIP_MTIP | MIP_MEIP;
    asm volatile("csrw mie, %[reg]" : : [reg] "r"((uint32_t)mie));
    asm volatile("csrsi mstatus, 8"); // MSTATUS_MIE

    // Jump to main application
    printk("launch main!\n");

    // parse main application args
    char **argv;
    fprintf(stdout, "zeptos # ");
    for (int i = 0; i < _argc; i++) {
        fprintf(stdout, "%s ", _argv[i]);
    }
    fprintf(stdout, "\n");

    int ret;
    ret = main(_argc, _argv);

    printk("Shutdown...\n");

    // sleep
    exit(ret);
}
