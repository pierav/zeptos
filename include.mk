ifndef ZETPOS_PATH
$(error You must define ZETPOS_PATH)
endif

ifndef ZEPTOS_RAMFS_PATH
ZEPTOS_RAMFS_PATH	= $(ZETPOS_PATH)/ramfs
ZEPTOS_RAMFS_CPIO 	= $(ZETPOS_PATH)/build/ramfs.cpio
ZEPTOS_RAMFS_OBJ 	= $(ZETPOS_PATH)/build/ramfs.o
else
ZEPTOS_RAMFS_CPIO 	= $(CURDIR)/build/ramfs.cpio
ZEPTOS_RAMFS_OBJ 	= $(CURDIR)/build/ramfs.o
endif

# Default rule
_all: all

###
# Filesystem image
###

# Create CPIO with the new (SVR4) portable format
$(ZEPTOS_RAMFS_CPIO): $(ZEPTOS_RAMFS_PATH)
	@mkdir -p $(@D)
	cd $^ && find . | cpio -oV -H newc > $@

# Create ELF fs independent
# This must create symbols:
# D _binary_ramfs_cpio_end
# A _binary_ramfs_cpio_size
# D _binary_ramfs_cpio_start
$(ZEPTOS_RAMFS_OBJ): $(ZEPTOS_RAMFS_CPIO)
	cd $(^D) && $(LD) -r -b binary $(notdir $^) -o $@
	nm $@


# * Generic
# 	- Plateform independant
MODEL 			?= generic

ZEPTOS_LIB		:= $(ZETPOS_PATH)/build/libzeptos_$(MODEL).a
ZEPTOS_INCLUDE 	:= $(ZETPOS_PATH)/include

$(ZEPTOS_LIB): force
	make -C $(ZETPOS_PATH)


XLEN			:= 64
PREFIX 			:= riscv$(XLEN)-unknown-elf-
CC 				:= $(PREFIX)gcc
LD 				:= $(PREFIX)ld

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
			 -nostartfiles -nodefaultlibs \
			  -lm -T $(ZETPOS_PATH)/src/boot/link.ld


###
# Force
###

force:
	@: