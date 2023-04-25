ZeptOS
======

Zeptos is a lightweight bare-metal C library conforming to a reduced set ot the IEEE POSIX 1003.1 standard.

The motivation is to run libc-dependent applications without using a greedy operating system.

Zeptos can be used as a starting point for bare metal RISC-V programs that require threads, malloc, printf and basic IO.

# Usage

```sh
# Build (zeptos is arch independant)
make
# Test with qemu virt
qemu-system-riscv64 -nographic -machine virt -m 1g -smp 4 -bios ./build/libzeptos
# Test with spike
spike -p4 ./build/libzeptos

# ,---,         .    ,-.   ,-.  
#    /          |   /   \ (   ` 
#   /   ,-. ;-. |-  |   |  `-.  
#  /    |-' | | |   \   / .   ) 
# '---' `-' |-' `-'  `-'   `-'  
#           '       v0.0.0
# 
# [0000000000044909/0] init::_init: Boot core [0] / DTB=@00001020
# [0000000000045e13/1] init::_init: DTB[0] = edfe0dd0
# [0000000000046f62/2] init::_init: 10000000.uart ns16550a driver 00000000/00000001
# [0000000000048a21/3] init::_init: Init BSS [80006d20: 80006e2c]...
# [000000000004a010/4] init::_init: Setup dyn memory...
# [0000000000080e85/5] init::_init: 02000000.riscv,clint0 Initialise
# [000000000008243a/6] init::_init: 02000000.riscv,clint0 register IT 00000007 with clint_timer_it
# [00000000000842d4/7] init::_init: 02000000.riscv,clint0 register IT # 00000003 with clint_ipi_it
# [00000000000860e2/8] init::_init: Enable ITs
# MINT
# [0000000000087260/9] init::_init: launch main!
# [0000000000088252/01] pthread::pthread_create: 0 : 80001800 ( 80006e48 )
# [0000000000089a4d/11] pthread::pthread_create: 1 : 80001800 ( 80006e58 )
# [000000000008b246/21] pthread::pthread_create: 2 : 80001800 ( 80006e68 )
# [000000000008ca40/31] pthread::pthread_create: 3 : 80001800 ( 80006e78 )
# [000000000008e23d/41] pthread::pthread_join: 0

```