ifndef ZETPOS_PATH
$(error You must define ZETPOS_PATH)
endif

# * Generic
# 	- Plateform independant
MODEL 			?= generic

ZEPTOS_LIB		:= $(ZETPOS_PATH)/build/libzeptos_$(MODEL).a
ZEPTOS_INCLUDE 	:= $(ZETPOS_PATH)/include

XLEN			:= 64
PREFIX 			:= riscv$(XLEN)-unknown-elf-
CC 				:= $(PREFIX)gcc

ZEPTOS_CFLAGS 	= -DPREALLOCATE=1 -mcmodel=medany \
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
			  	  -I$(ZEPTOS_INCLUDE) \
			  	  $(INCS)

ZEPTOS_LDFLAGS 	= $(INCS) -static \
			  -nostdlib -nostartfiles \
			  -lm -T $(ZETPOS_PATH)/src/boot/link.ld

