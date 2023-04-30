ifndef ZETPOS_PATH
ZETPOS_PATH=$(CURDIR)
endif

XLEN		:= 64
PREFIX 		:= riscv$(XLEN)-unknown-elf-
CC 			:= $(PREFIX)gcc

CFLAGS 		= -DPREALLOCATE=1 -mcmodel=medany \
			  -MMD -MP \
			  -g \
			  -static -std=gnu99 -O2 \
			  -Wall -Wextra \
			  -Wno-sign-compare \
			  -Wno-unused-variable \
			  -Wno-unused-parameter \
			  -nostdinc \
			  -fno-common -fno-builtin-printf -fno-builtin \
			  -march=rv64imafdc -mabi=lp64d \
			  -fno-tree-loop-distribute-patterns \
			  $(INCS)

LDFLAGS 	= $(INCS) -static \
			  -nostdlib -nostartfiles \
			  -lm -T $(ZETPOS_PATH)/src/boot/link.ld