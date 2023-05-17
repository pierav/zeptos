###
# Setup
###

###
# Files
###

incdir 		= $(shell find src -type d) include config
INCS 		= $(foreach dir, ${incdir}, -I$(dir))

export ZETPOS_PATH=$(CURDIR)
export ZEPTOS_RAMFS_PATH=$(CURDIR)/ramfs

ZEPTOS_RAMFS_CPIO 	= $(ZETPOS_PATH)/build/ramfs.cpio
ZEPTOS_RAMFS_OBJ 	= $(ZETPOS_PATH)/build/ramfs.o

include include.mk

SRC_C 		:= $(shell find ./src -type f -name '*.c')
SRC_S 		:= $(shell find ./src -type f -name '*.S')

OBJ 		:= $(subst src/,build/,$(SRC_C:.c=_$(MODEL).o)) \
			   $(subst src/,build/,$(SRC_S:.S=_$(MODEL).o)) \
			   $(ZEPTOS_RAMFS_OBJ)

LIB 		= build/libzeptos_$(MODEL).a
EXEC 		= build/libzeptos_$(MODEL)

# Default rule
all: $(EXEC) $(LIB)

###
# Configuration
###

export KCONFIG_CONFIG = config/$(MODEL).config
CONFIG_HEADER = config/config.h

$(KCONFIG_CONFIG):
	@echo >&2 '***'
	@echo >&2 '*** Configuration file "$@" not found!'
	@echo >&2 '***'
	@echo >&2 '*** Please run some configurator (e.g. "oldconfig" or'
	@echo >&2 '*** "menuconfig" or "xconfig").'
	@echo >&2 '***'
	@/bin/false

menuconfig:
	menuconfig

genconfig: $(CONFIG_HEADER)

$(CONFIG_HEADER): $(KCONFIG_CONFIG) force
	genconfig --header-path $(CONFIG_HEADER)

.phony: menuconfig genconfig

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

###	
# Build
###

CFLAGS 		= $(ZEPTOS_CFLAGS)
LDFLAGS 	= $(ZEPTOS_LDFLAGS)

build/%_$(MODEL).o: src/%.c $(CONFIG_HEADER)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -D__MODULE__=\"$(notdir $(basename $<))\" -o $@ -c $<

build/%_$(MODEL).o: src/%.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

$(LIB): $(OBJ)
	ar rc $@ $+
	ranlib $@

$(EXEC): $(LIB) build/boot/main_$(MODEL).o
	$(CC) $(LDFLAGS) -o $@ $^


-include $(OBJ:.o=.d)

clean:
	rm -r build

.phony: run run_spike run_qemu

###
# Run
###

NR_CPUS = 8 

run: run_spike

run_spike: $(EXEC)
	spike --isa=rv$(XLEN)gc -p$(NR_CPUS) $^

run_qemu: $(EXEC)
	qemu-system-riscv$(XLEN) -nographic -machine virt -m 1g -smp 5 -bios $^

run_gdb: $(EXEC)
	echo "TODO"

###
# Misc
###

force:
	@: