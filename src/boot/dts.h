#include "fdt.h"
#include "libfdt.h"
#include "libfdt_env.h"

int fdt_parse_ns16550(void *fdt, uintptr_t *ns16550_addr,
                      uint32_t *reg_shift, uint32_t *reg_io_width,
                      const char *compatible);

int fdt_parse_clint(void *fdt, uintptr_t *clint_addr,
                    const char *compatible);