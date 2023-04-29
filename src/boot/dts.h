#include "fdt.h"
#include "libfdt.h"
#include "libfdt_env.h"

int fdt_parse_ns16550(void *fdt, uintptr_t *ns16550_addr, uint32_t *reg_shift,
                      uint32_t *reg_io_width, const char *compatible);

int fdt_parse_clint(void *fdt, uintptr_t *clint_addr, const char *compatible);

int fdt_parse_cpus(void *fdt, int *nr_cpus, const char **compatible,
                   const char **isa, const char **mmu);

/**
 * Fast BOOT
 */

#ifdef CONFIG_DTS_STATIC

CONFIG_DTS_STATIC
CONFIG_DTS_NS16550_ADDR
CONFIG_DTS_NS16550_REG_SHIFT
CONFIG_DTS_NS16550_REG_WIDTH
CONFIG_DTS_CLINT_ADDR
CONFIG_DTS_CPUS_COMPATIBLE
CONFIG_DTS_CPUS_ISA
CONFIG_DTS_CPUS_MMU

#error TODO

int fdt_parse_ns16550(void *fdt, uintptr_t *ns16550_addr, uint32_t *reg_shift,
                      uint32_t *reg_io_width, const char *compatible) {
    *ns16550_addr = CONFIG_DTS_NS16550_ADDR;
    *reg_shift = CONFIG_DTS_NS16550_REG_SHIFT;
    *reg_io_width = CONFIG_DTS_NS16550_REG_WIDTH;
    return 0;
}

int fdt_parse_clint(void *fdt, uintptr_t *clint_addr, const char *clint_addr) {
    *clint_addr = CONFIG_DTS_CLINT_ADDR;
    return 0;
}

int fdt_parse_cpus(void *fdt, int *nr_cpus, const char **compatible,
                   const char **isa, const char **mmu) {
    *nr_cpus = CONFIG_NR_CPUS; // Best fit
    *compatible = CONFIG_DTS_CPUS_COMPATIBLE;
    *isa = CONFIG_DTS_CPUS_ISA;
    *mmu = CONFIG_DTS_CPUS_MMU;
    return 0;
}

#endif
