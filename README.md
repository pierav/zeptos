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

# [0000000000044810/0] init::_init: Boot core [0] / DTB=@00001020
# [0000000000045cb7/1] init::_init: DTB[0] = edfe0dd0
# [0000000000046d9a/2] init::_init: 10000000.uart ns16550a driver 00000000/00000001
# [000000000004880f/3] init::_init: Init BSS [80005978: 80005c68]...
# [0000000000049d9b/4] init::_init: Setup dyn memory...
# [0000000000080b9e/5] init::_init: 02000000.riscv,clint0 Initialise
# [00000000000820f2/6] init::_init: 02000000.riscv,clint0 register IT 00000007 with clint_timer_it
# [0000000000083f31/7] init::_init: 02000000.riscv,clint0 register IT 00000003 with clint_ipi_it
# [0000000000085ce0/8] init::_init: Enable ITs
# [0000000000086bea/9] init::_init: launch main!
# HELLO WORLD @@@ZEPTOS@@@
# [000000000008833f/01] pthread::pthread_create: 0 : 800048f8 ( 80005c80 )
# [0000000000089b2b/11] pthread::_pthread_attach: 0
# Thread 0: top of stack near 0x0000000080025a18; argv_string=Thread0
# [000000000008c127/21] pthread::pthread_create: 1 : 800048f8 ( 80005c90 )
# [000000000008d8cf/31] pthread::pthread_create: 2 : 800048f8 ( 80005ca0 )
# [000000000008f074/41] pthread::pthread_create: 3 : 800048f8 ( 80005cb0 )
# [000000000009081b/61] pthread::pthread_join: 0
# Joined with thread 0; returned value was THREAD0
# [00000000000926c2/91] pthread::pthread_join: 1
# [000000000008ed6c/51] pthread::_pthread_attach: 1
# Thread 1: top of stack near 0x0000000080065b18; argv_string=Victim
# [00000000000900f4/71] pthread::_pthread_attach: 2
# Thread 2: top of stack near 0x00000000800a5b18; argv_string=Hello World
# [000000000009147c/81] pthread::_pthread_attach: 3
# Thread 3: top of stack near 0x00000000800e5b18; argv_string=Teddy
# Joined with thread 1; returned value was VICTIM
# [000000000009809c/02] pthread::pthread_join: 2
# Joined with thread 2; returned value was HELLO WORLD
# [000000000009a066/12] pthread::pthread_join: 3
# Joined with thread 3; returned value was TEDDY


```
