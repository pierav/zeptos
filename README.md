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
#           '       v0.0.0
#
# [0000000000069dd9/0][0] init::_init: Boot core [0] / DTB=(0x00001020)=0xedfe0dd0
# [000000000006ba44/1][0] init::_init: 10000000.uart ns16550a driver 00000000/00000001
# [000000000006d848/2][0] init::_init: Init BSS [80005ed0: 800061d4]...
# [000000000006f0f8/3][0] init::_init: Setup dyn memory...
# [00000000000c9d1e/4][0] init::_init: Catch 8 cpu [riscv, rv64imafdc, riscv,sv57]
# [00000000000cb931/5][0] init::_init: Initialise pthread with 8 cpus
# [0000000000127f68/6][0] init::_init: 02000000.riscv,clint0 Initialise
# [00000000001297e6/7][0] init::_init: 02000000.riscv,clint0 register IT 00000007 with clint_timer_it
# [000000000012ba1c/8][0] init::_init: 02000000.riscv,clint0 register IT 00000003 with clint_ipi_it
# [000000000012dbae/9][0] init::_init: Enable ITs
# [000000000012ed44/01][0] init::_init: launch main!
# HELLO WORLD @@@ZEPTOS@@@
# [000000000013085a/11][0] pthread::pthread_create: 0 : 80004c9e ( 800061f0 )
# [000000000013235b/21][0] pthread::pthread_create: 1 : 80004c9e ( 80006200 )
# [0000000000133e5d/41][0] pthread::pthread_create: 2 : 80004c9e ( 80006210 )
# [000000000013595e/51][0] pthread::pthread_create: 3 : 80004c9e ( 80006220 )
# [000000000013745f/71][0] pthread::pthread_join: 7
# [00000000001374be/91][4] pthread::_pthread_attach: 4
# Thread 3: top of stack near 0x0000000080126098; argv_string=Teddy
# [0000000000136136/81][5] pthread::_pthread_attach: 5
# Thread 2: top of stack near 0x0000000080166098; argv_string=Hello World
# [0000000000134dae/61][6] pthread::_pthread_attach: 6
# Thread 1: top of stack near 0x00000000801a6098; argv_string=Victim
# [000000000013269e/31][7] pthread::_pthread_attach: 7
# Thread 0: top of stack near 0x00000000801e6098; argv_string=Thread0
# Joined with thread 0; returned value was THREAD0
# [0000000000143614/02][0] pthread::pthread_join: 6
# Joined with thread 1; returned value was VICTIM
# [0000000000145880/12][0] pthread::pthread_join: 5
# Joined with thread 2; returned value was HELLO WORLD
# [0000000000147c6d/22][0] pthread::pthread_join: 4
# Joined with thread 3; returned value was TEDDY

```
