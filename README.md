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
spike -p8 ./build/libzeptos


# ,---,         .    ,-.   ,-.  
#    /          |   /   \ (   ` 
#   /   ,-. ;-. |-  |   |  `-.  
#  /    |-' | | |   \   / .   ) 
# '---' `-' |-' `-'  `-'   `-'  
#           '       "v0.0.0"
#
# [  290038][0] init::_init: Boot core [0] / DTB=(0x80027520)=0xd00dfeed
# [  296438][0] init::_init: 10000000.uart ns16550a driver  0/ 1
# [  302643][0] init::_init: TOS = 80027f88
# [  306681][0] init::_init: Init BSS [80007cc0: 80007fcc]...
# [  312124][0] init::_init: Setup dyn memory...
# [  535423][0] init::_init: Catch 4 cpu [riscv, rv64imafdc, riscv,sv57]
# [  541467][0] init::_init: Initialise pthread with 4 cpus
# [  766903][0] init::_init: 2000000.riscv,clint0 Initialise
# [  771986][0] init::_init: 2000000.riscv,clint0 register IT 7 with clint_timer_it
# [  778517][0] init::_init: 2000000.riscv,clint0 register IT 3 with clint_ipi_it
# [  784960][0] init::_init: Enable ITs
# [  788320][0] init::_init: launch main!
# zeptos # zeptos hello world 
# [  794480][0] pthread::pthread_create: 8000642e ( 88000018 ) PID=1
# [  800810][0] pthread::pthread_create: 8000642e ( 88000028 ) PID=2
# [  807149][0] pthread::pthread_create: 8000642e ( 88000038 ) PID=3
# [  813443][0] pthread::pthread_join: 1
# [  800070][1] pthread::_pthread_attach: 1
# Thread 0: TOS~=0000000080067e48; malloc[@88000060] argv_string=11aaaaa11->11AAAAA11
# [  805070][2] pthread::_pthread_attach: 2
# Thread 1: TOS~=00000000800a7e48; malloc[@88000088] argv_string=22bbbbb22->22BBBBB22
# [  815070][3] pthread::_pthread_attach: 3
# Thread 2: TOS~=00000000800e7e48; malloc[@880000b0] argv_string=33ccccc33->33CCCCC33
# [  828266][0] pthread::pthread_join: 2
# [  852014][0] pthread::pthread_join: 3
# Joined with thread 0; returned value was 11AAAAA11 @88000060
# Joined with thread 1; returned value was 22BBBBB22 @88000088
# Joined with thread 2; returned value was 33CCCCC33 @880000b0
# [  868996][0] init::_init: Shutdown...



```


# On hold

## +++

* Log level
* more pthread support
* free() bugged

## +/-

* memory from dtb
* fdt parsing for poweroff
* printk buffer

## ---

* Versions automation

# Deps

https://pypi.org/project/kconfiglib/