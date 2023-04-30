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

#
# ,---,         .    ,-.   ,-.  
#    /          |   /   \ (   ` 
#   /   ,-. ;-. |-  |   |  `-.  
#  /    |-' | | |   \   / .   ) 
# '---' `-' |-' `-'  `-'   `-'  
#           '       v0.0.0
#
# [0000000038891a46/0][0] init::_init: Boot core [0] / DTB=(0xbfe00000)=0xedfe0dd0
# [0000000038e64938/1][0] init::_init: 10000000.uart ns16550a driver 00000000/00000001
# [00000000391949ee/2][0] init::_init: TOS = 800264d8
# [00000000392de540/3][0] init::_init: Init BSS [80006208: 80006514]...
# [00000000393be546/4][0] init::_init: Setup dyn memory...
# [00000000397131a6/5][0] init::_init: Catch 8 cpu [riscv, rv64imafdch_zicsr_zifencei_zihintpause_zba_zbb_zbc_zbs_sstc, riscv,sv48]
# [0000000039890e4c/6][0] init::_init: Initialise pthread with 8 cpus
# [0000000039b60470/7][0] init::_init: 02000000.riscv,clint0 Initialise
# [0000000039c3cd3e/8][0] init::_init: 02000000.riscv,clint0 register IT 00000007 with clint_timer_it
# [0000000039d67dd4/9][0] init::_init: 02000000.riscv,clint0 register IT 00000003 with clint_ipi_it
# [0000000039e88f4a/01][0] init::_init: Enable ITs
# [0000000039f8deb0/11][0] init::_init: launch main!
# HELLO WORLD @@@ZEPTOS@@@
# [000000003a0fbcea/21][0] pthread::pthread_create: 80004f2c ( 88000018 ) PID=1
# [000000003a20e78e/31][0] pthread::pthread_create: 80004f2c ( 88000028 ) PID=2
# [000000003a2e4e18/41][0] pthread::pthread_create: 80004f2c ( 88000038 ) PID=3
# [000000003a3b4e10/51][0] pthread::pthread_create: 80004f2c ( 88000048 ) PID=4
# [000000003a2e5c32/61][1] pthread::_pthread_attach: 1
# [000000003a49a4a0/71][0] pthread::pthread_join: 1
# [000000003a5ce82a/81][2] pthread::_pthread_attach: 2
# [000000003a6871c6/02][3] pthread::_pthread_attach: 3
# [000000003a5d3806/91][4] pthread::_pthread_attach: 4
# Thread 3: TOS~=0x0000000080126388; malloc[@880000e8] argv_string=44ddddd44->44DDDDD44
# Thread 2: TOS~=0x00000000800e6388; malloc[@880000c0] argv_string=33ccccc33->33CCCCC33
# Thread 0: TOS~=0x0000000080066388; malloc[@88000098] argv_string=11aaaaa11->11AAAAA11
# Thread 1: TOS~=0x00000000800a6388; malloc[@88000070] argv_string=22bbbbb22->22BBBBB22
# [000000003ac2a0da/12][0] pthread::pthread_join: 2
# [000000003adb914c/22][0] pthread::pthread_join: 3
# [000000003ae40ca4/32][0] pthread::pthread_join: 4
# Joined with thread 0; returned value was 11AAAAA11 @88000098
# Joined with thread 1; returned value was 22BBBBB22 @88000070
# Joined with thread 2; returned value was 33CCCCC33 @880000c0
# Joined with thread 3; returned value was 44DDDDD44 @880000e8
# [000000003b1a3d76/42][0] init::_init: Shutdown...


```


# On hold

# Deps

https://pypi.org/project/kconfiglib/