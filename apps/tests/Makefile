ZETPOS_PATH = $(CURDIR)/../..
ZEPTOS_RAMFS_PATH = $(CURDIR)/ramfs

include $(ZETPOS_PATH)/include.mk

SRC = $(wildcard src/*.c)
EXECS = $(subst src/,build/,$(SRC:.c=.riscv))

all: $(EXECS)

build/%.riscv: src/%.c $(ZEPTOS_LIB) $(ZEPTOS_RAMFS_OBJ)
	@mkdir -p $(@D)
	$(CC) $(ZEPTOS_CFLAGS) $(ZEPTOS_LDFLAGS) -o $@ $^

.phony: run_all

run_all: $(EXECS)
	for test in $^; do echo $$test && spike -p8 $$test; done
