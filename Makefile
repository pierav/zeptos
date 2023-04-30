###
# Setup
###

# * Generic
# 	- Plateform independant
MODEL = generic

###
# Files
###

incdir 		= $(shell find src -type d) include config
INCS 		= $(foreach dir, ${incdir}, -I$(dir))

include include.mk

SRC_C 		:= $(shell find ./src -type f -name '*.c')
SRC_S 		:= $(shell find ./src -type f -name '*.S')

OBJ 		:= $(subst src/,build/,$(SRC_C:.c=.o)) \
			   $(subst src/,build/,$(SRC_S:.S=.o))

LIB 		= build/libzeptos.a
EXEC 		= build/libzeptos

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

genconfig: $(KCONFIG_CONFIG)
	genconfig --header-path $(CONFIG_HEADER)

###	
# Build
###

build/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -D__MODULE__=\"$(notdir $(basename $<))\" -o $@ -c $<

build/%.o: src/%.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

$(LIB): $(OBJ)
	ar rc $@ $+
	ranlib $@

$(EXEC): $(LIB) build/boot/main.o
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
