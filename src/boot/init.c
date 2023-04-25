#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "printk.h"

#define MCAUSE_INT                 63
#define MCAUSE_MISALIGNED_FETCH    (((uint64_t)0 << MCAUSE_INT) | 0)
#define MCAUSE_FAULT_FETCH         (((uint64_t)0 << MCAUSE_INT) | 1)
#define MCAUSE_ILLEGAL_INSTRUCTION (((uint64_t)0 << MCAUSE_INT) | 2)
#define MCAUSE_BREAKPOINT          (((uint64_t)0 << MCAUSE_INT) | 3)
#define MCAUSE_MISALIGNED_LOAD     (((uint64_t)0 << MCAUSE_INT) | 4)
#define MCAUSE_FAULT_LOAD          (((uint64_t)0 << MCAUSE_INT) | 5)
#define MCAUSE_MISALIGNED_STORE    (((uint64_t)0 << MCAUSE_INT) | 6)
#define MCAUSE_FAULT_STORE         (((uint64_t)0 << MCAUSE_INT) | 7)
#define MCAUSE_ECALL_U             (((uint64_t)0 << MCAUSE_INT) | 8)
#define MCAUSE_ECALL_S             (((uint64_t)0 << MCAUSE_INT) | 9)
#define MCAUSE_ECALL_M             (((uint64_t)0 << MCAUSE_INT) | 11)
#define MCAUSE_PAGE_FAULT_INST     (((uint64_t)0 << MCAUSE_INT) | 12)
#define MCAUSE_PAGE_FAULT_LOAD     (((uint64_t)0 << MCAUSE_INT) | 13)
#define MCAUSE_PAGE_FAULT_STORE    (((uint64_t)0 << MCAUSE_INT) | 15)
#define MCAUSE_INTERRUPT           (((uint64_t)1 << MCAUSE_INT))

const char* DISPLAY_MCAUSE[] = {
    [0]="MISALIGNED_FETCH",
    [1]="FAULT_FETCH",
    [2]="ILLEGAL_INSTRUCTION",
    [3]="BREAKPOINT",
    [4]="MISALIGNED_LOAD",
    [5]="FAULT_LOAD",
    [6]="MISALIGNED_STORE",
    [7]="FAULT_STORE",
    [8]="ECALL_U",
    [9]="ECALL_S",
    [11]="ECALL_M",
    [12]="PAGE_FAULT_INST",
    [13]="PAGE_FAULT_LOAD",
    [15]="PAGE_FAULT_STORE"
};

#define NB_EXCEPT 16
#define NB_IT 64

void handle_trap_none(uintptr_t cause, uintptr_t epc){
  uintptr_t mtval;
  asm volatile ("csrr %[reg], mtval" : [reg] "=r" (mtval));
  printf("Bad trap: %x epc=%lx tval=%lx\n", cause, epc, mtval);
  exit(1000+cause);
}

void (*handle_it_arr[NB_IT])(uintptr_t, uintptr_t) = { handle_trap_none };
void (*handle_except_arr[NB_EXCEPT])(uintptr_t, uintptr_t) = { handle_trap_none };

uintptr_t handle_trap(uintptr_t cause, uintptr_t epc, uintptr_t regs[32]){
  if (cause >= MCAUSE_INTERRUPT) { // Interrupt
      uint64_t it = (cause - MCAUSE_INTERRUPT);
      // printk("interruption %x, pc %x\n",it , epc);
      handle_it_arr[it](cause, epc);
  } else { // Exception
      // printk("exception %s, pc %x\n", DISPLAY_MCAUSE[cause], epc);
      handle_except_arr[cause](cause, epc);
  }
  (void) regs;
  return epc;
}

int __attribute__((weak)) main(int argc, char** argv){
  if(argc == 2){
    printf("%s >>> %s\n", argv[0], argv[1]);
  }
  printf("Implement main()!\n");
  return -1;
}

#include "drivers/ns16550a.h"
#include "banner.h"

#include "libfdt.h"
#include "libfdt_env.h"
#include "fdt.h"
#include "dts.h"

#include "clint.h"
#define ARIANE_UART_ADDR			  0x10000000
#define ARIANE_UART_FREQ			  50000000
#define ARIANE_UART_BAUDRATE		115200
#define ARIANE_UART_REG_SHIFT		2
#define ARIANE_UART_REG_WIDTH	  4


#define  CLINT_INTERRUPT_PERIOD  1200000   

#define NR_CPUS 64

void park(){
  while(1);
}

extern char _end;
extern char _bss_start;
extern char _bss_end;

void _trap(int i){
  while(1){
    i = i * 1;
    asm volatile ("wfi");
  }
}

void _init(uint64_t cid, uint64_t dtb){
    // Init trap handler
    for(int i = 0; i < NB_EXCEPT; i++){
      handle_except_arr[i] = handle_trap_none;
    }
    for(int i = 0; i < NB_IT; i++){
      handle_it_arr[i] = handle_trap_none;
    }

    // Only core 0 boots
    if(cid != 0){
      park();
    }

    // Check if dtb is valid
    void *fdt = (void*)dtb;
    if (fdt_magic(fdt) != FDT_MAGIC){
      _trap(2);
      printk("Invalid FDT");
      exit(1);
    }

    // The first step is to initialise the Serial device  
    uintptr_t ns16550_addr;
    uint32_t reg_shift, reg_io_width;
    const char *compatible = "ns16550a";
    int res = fdt_parse_ns16550(fdt, &ns16550_addr, &reg_shift, &reg_io_width, compatible);
    if(res == 0){
      uart8250_init((void*)ns16550_addr,
                  ARIANE_UART_FREQ,
                  ARIANE_UART_BAUDRATE,
                  reg_shift,
                  reg_io_width);
      _init_serial(uart8250_getc, uart8250_putc);
    }
    else {
      exit(1);
    }

    // Print banner
    puts(ZEPTOS_BANNER);
    printk("Boot core [%d] / DTB=@%x\n", cid, fdt);
    printk("DTB[0] = %x\n",  *(uint64_t*)fdt);
    printk("%x.uart %s driver %2x/%2x\n", ns16550_addr, compatible, reg_shift, reg_io_width);   

    // Init memory
    printk("Init BSS [%x: %x]...\n", &_bss_start, &_bss_end);
    // memset(&_bss_start, 0, &_bss_end - &_bss_start);
    printk("Setup dyn memory...\n");
    _malloc_addblock(&_end, 128*1024); // Lest add 128 kB

    // Parse DTS
    // printk("fdt_version(p)=%d\n", fdt_version(fdt));
    // printk("fdt_last_comp_version(p)=%d\n", fdt_last_comp_version(fdt));
    // printk("fdt_totalsize(p)=%d\n", fdt_totalsize(fdt));
    // printk("fdt_off_dt_struct(p)=%d\n", fdt_off_dt_struct(fdt));
    // printk("fdt_off_dt_strings(p)=%d\n", fdt_off_dt_strings(fdt));	

    // Get all CPUS


    // Plic initialisation
    // Todo

    // Clint initialisation
    uintptr_t clint_addr;
    char * clint_compatible = "riscv,clint0";
    if(fdt_parse_clint(fdt, &clint_addr, clint_compatible) != 0){
      panic("No clint found in DTS\n");
    }
    printk("%x.%s Initialise\n", clint_addr, clint_compatible);   
    clint_init(clint_addr, CLINT_INTERRUPT_PERIOD);
    printk("%x.%s register IT %x with clint_timer_it\n", clint_addr, clint_compatible, 7);   
    printk("%x.%s register IT %x with clint_ipi_it\n", clint_addr, clint_compatible, 3);   
    handle_it_arr[7] = clint_timer_it;
    handle_it_arr[3] = clint_ipi_it;
    // register IT TODO FDT id

    // machine interrupt enable
    printk("Enable ITs\n");
    asm volatile ("csrw mie, %[reg]" : : [reg] "r" ((uint32_t) 0x80));
    asm volatile ("csrsi mstatus, 8");

    // Jump to main application
    char *argv[] = {"zeptos", "Hello World"};
    printk("launch main!\n");

    int ret = main(2, argv);

    // sleep
    while (1){
      asm volatile ("wfi");
    }

    exit(ret);
}