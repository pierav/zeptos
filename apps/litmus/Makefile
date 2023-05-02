ZETPOS_PATH = $(CURDIR)/../..
LITMUS_PATH = $(CURDIR)/litmus-tests-riscv

include $(ZETPOS_PATH)/include.mk

# https://diy.inria.fr/doc/index.html

all: compare

###
# Litmus build
###


LITMUS = litmus7
LITMUS_CONFIG = ./zeptos.cfg
LITMUS_OPTS = -avail 2 
# -excl gcc.excl -excl instructions.excl

$(LITMUS_PATH):
	git clone https://github.com/litmus-tests/litmus-tests-riscv.git $@

LITMUS_SET = non-mixed-size/CO/@all

.phony: build_src
build_src: $(LITMUS_CONFIG) $(LITMUS_PATH)
	@mkdir -p build
	$(LITMUS) -mach $< $(LITMUS_OPTS) -o build "$(LITMUS_PATH)/tests/$(LITMUS_SET)"


###
# build
###

CFLAGS 		= $(ZEPTOS_CFLAGS) -DCPUS_DEFINED=0 -DASS
LDFLAGS		= $(ZEPTOS_LDFLAGS)

SRC 		= $(wildcard build/*.c)
OBJ 		= $(SRC:.c=.o) build/args.o

INCS = -I/$(ZETPOS_PATH)/include -Ibuild

EXEC = build/exec

all: $(EXEC)

build/%.o: build/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

$(EXEC): $(OBJ) $(ZETPOS_PATH)/build/libzeptos_generic.a
	$(CC) $(LDFLAGS) -o $@ $^


###
# Run with qemu
###

QEMU = qemu-system-riscv64
QEMU_OPTS = -nographic -machine virt -m 1g -smp 8


build/qemu.log: $(EXEC)
	$(QEMU) $(QEMU_OPTS) -bios $^ | tee $@

###
# mcompare7
###

COMPARE_REF = $(LITMUS_PATH)/model-results/flat.logs
MCOMPARE = mcompare7

build/compare.log: build/qemu.log $(COMPARE_REF)
	mcompare7 -nohash $^  | tee $@

.phony: build run compare

build: $(EXEC)
run: build/qemu.log
compare: build/compare.log

###
# clean
###

.phony: clean
clean:
	rm -rf qemu.log compare.log build
