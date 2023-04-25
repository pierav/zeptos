SRC_C 		:= $(shell find ./src -type f -name '*.c')
SRC_S 		:= $(shell find ./src -type f -name '*.S')

OBJ 		:= $(subst src/,build/,$(SRC_C:.c=.o)) \
			   $(subst src/,build/,$(SRC_S:.S=.o))

LIB 		= build/libzeptos.a
EXEC 		= build/libzeptos

XLEN		:= 64
PREFIX 		:= riscv$(XLEN)-unknown-elf-
CC 			:= $(PREFIX)gcc
OBJDUMP 	:= $(RISCV_PREFIX)objdump
OBJCOPY 	:= $(RISCV_PREFIX)objcopy

incdir 		:= $(shell find src -type d) include
INCS 		:= $(foreach dir, ${incdir}, -I$(dir))
CFLAGS 		:= -DPREALLOCATE=1 -mcmodel=medany \
			  -MMD -MP \
			  -g \
			  -static -std=gnu99 -O2 \
			  -Wall -Wextra \
			  -Wno-sign-compare \
			  -nostdinc \
			  -ffast-math -fno-common -fno-builtin-printf -fno-builtin \
			  -fno-tree-loop-distribute-patterns \
			  -DCONFIG_ZEPTOS_BANNER_EXTRA=v0.0.0 \
			  $(INCS)


LDFLAGS 	:= $(INCS) -static \
			  -nostdlib -nostartfiles \
			  -lm -T src/boot/link.ld
SIM 		:= spike --isa=rv$(XLEN)gc


all: $(EXEC)

build/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -D__MODULE__=\"$(notdir $(basename $<))\" -o $@ -c $<

build/%.o: src/%.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

-include $(OBJ:.o=.d)

clean:
	rm -r build