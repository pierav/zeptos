#include "dts.h"
#include "fdt.h"
#include "libfdt.h"
#include "libfdt_env.h"
#include "printk.h"
#include <assert.h>

static int fdt_get_node_addr_size(void *fdt, int node, uintptr_t *addr,
                                  unsigned long *size, const char *field) {
    int            parent, len, i;
    int            cell_addr, cell_size;
    const fdt32_t *prop_addr, *prop_size;
    uint64_t       temp = 0;

    parent = fdt_parent_offset(fdt, node);
    if (parent < 0)
        return parent;

    cell_addr = fdt_address_cells(fdt, parent);
    if (cell_addr < 1)
        return -1;

    cell_size = fdt_size_cells(fdt, parent);
    if (cell_size < 0)
        return -1;

    if (!field)
        return -1;

    prop_addr = (fdt32_t *)fdt_getprop(fdt, node, field, &len);
    if (!prop_addr)
        return -1;
    prop_size = prop_addr + cell_addr;

    if (addr) {
        for (i = 0; i < cell_addr; i++)
            temp = (temp << 32) | fdt32_to_cpu(*prop_addr++);
        *addr = temp;
    }
    temp = 0;

    if (size) {
        for (i = 0; i < cell_size; i++)
            temp = (temp << 32) | fdt32_to_cpu(*prop_size++);
        *size = temp;
    }

    return 0;
}

static int check_cpu_node(void *fdt, int cpu_offset) {
    int         len;
    const void *prop;

    if (!fdt || cpu_offset < 0)
        return -1;

    prop = fdt_getprop(fdt, cpu_offset, "device_type", &len);
    if (!prop || !len)
        return -1;
    if (strncmp((char *)prop, "cpu", strlen("cpu")))
        return -1;

    return 0;
}

int fdt_get_offset(void *fdt, const char *field) {
    return fdt_path_offset(fdt, field);
}

int fdt_get_first_subnode(void *fdt, int node) {
    return fdt_first_subnode(fdt, node);
}

int fdt_get_next_subnode(void *fdt, int node) {
    return fdt_next_subnode(fdt, node);
}

int fdt_parse_clint(void *fdt, uintptr_t *clint_addr, const char *compatible) {
    int nodeoffset, rc;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
    if (nodeoffset < 0)
        return nodeoffset;

    rc = fdt_get_node_addr_size(fdt, nodeoffset, clint_addr, NULL, "reg");
    if (rc < 0 || !clint_addr)
        return -1;

    return 0;
}

int fdt_parse_plic(void *fdt, uintptr_t *plic_addr, uint32_t *ndev,
                   const char *compatible) {
    int            nodeoffset, len, rc;
    const fdt32_t *ndev_p;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
    if (nodeoffset < 0)
        return nodeoffset;

    rc = fdt_get_node_addr_size(fdt, nodeoffset, plic_addr, NULL, "reg");
    if (rc < 0 || !plic_addr)
        return -1;

    ndev_p = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "riscv,ndev", &len);
    if (!ndev || !ndev_p)
        return -1;
    *ndev = fdt32_to_cpu(*ndev_p);

    return 0;
}

int fdt_parse_ns16550(void *fdt, uintptr_t *ns16550_addr, uint32_t *reg_shift,
                      uint32_t *reg_io_width, const char *compatible) {
    int            nodeoffset, len, rc;
    const fdt32_t *reg_p;

    nodeoffset = fdt_node_offset_by_compatible(fdt, -1, compatible);
    if (nodeoffset < 0)
        return nodeoffset;

    rc = fdt_get_node_addr_size(fdt, nodeoffset, ns16550_addr, NULL, "reg");
    if (rc < 0 || !ns16550_addr)
        return -1;

    reg_p = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-shift", &len);
    if (reg_shift) {
        if (reg_p) {
            *reg_shift = fdt32_to_cpu(*reg_p);
        } else {
            *reg_shift = 0;
        }
    }

    reg_p = (fdt32_t *)fdt_getprop(fdt, nodeoffset, "reg-io-width", &len);
    if (reg_io_width) {
        if (reg_p) {
            *reg_io_width = fdt32_to_cpu(*reg_p);
        } else {
            *reg_io_width = 1;
        }
    }

    return 0;
}

int fdt_parse_pmp_num(void *fdt, int cpu_offset, uintptr_t *pmp_num) {
    int rc;

    if ((rc = check_cpu_node(fdt, cpu_offset)) < 0)
        return rc;

    rc = fdt_get_node_addr_size(fdt, cpu_offset, pmp_num, NULL,
                                "riscv,pmpregions");
    if (rc < 0 || !pmp_num)
        return -1;

    return 0;
}

int fdt_parse_pmp_alignment(void *fdt, int cpu_offset, uintptr_t *pmp_align) {
    int rc;

    if ((rc = check_cpu_node(fdt, cpu_offset)) < 0)
        return rc;

    rc = fdt_get_node_addr_size(fdt, cpu_offset, pmp_align, NULL,
                                "riscv,pmpgranularity");
    if (rc < 0 || !pmp_align)
        return -1;

    return 0;
}

int fdt_parse_mmu_type(void *fdt, int cpu_offset, const char **mmu_type) {
    assert(mmu_type);

    int         len, rc;
    const void *prop;

    if ((rc = check_cpu_node(fdt, cpu_offset)) < 0)
        return rc;

    prop = fdt_getprop(fdt, cpu_offset, "mmu-type", &len);
    if (!prop || !len)
        return -1;

    *mmu_type = (const char *)prop;

    return 0;
}

int fdt_parse_cpus(void *fdt, int *nr_cpus) {
    *nr_cpus = 0;
    int off  = 0;
    do {
        off = fdt_node_offset_by_prop_value(fdt, off, "device_type", "cpu", 4);
        if (off > 0) {
            printk("Got CPU %d @%d\n", nr_cpus, off);
            const void *prop;
            int         len;

            prop = fdt_getprop(fdt, off, "compatible", &len);
            if (!prop || !len)
                return -1;
            printk("--> compatible: %s\n", (const char *)prop);

            prop = fdt_getprop(fdt, off, "riscv,isa", &len);
            if (!prop || !len)
                return -2;
            printk("--> riscv,isa: %s\n", (const char *)prop);

            prop = fdt_getprop(fdt, off, "mmu-type", &len);
            if (!prop || !len)
                return -3;
            printk("--> mmu-type: %s\n", (const char *)prop);

            *nr_cpus += 1;
        } else {
            break;
        }
    } while (off);
    return 0;
}