incdir 		= $(shell find src -type d) include
INCS 		= $(foreach dir, ${incdir}, -I$(dir))

include include.mk

SRC_C 		:= $(shell find ./src -type f -name '*.c')
SRC_S 		:= $(shell find ./src -type f -name '*.S')

OBJ 		:= $(subst src/,build/,$(SRC_C:.c=.o)) \
			   $(subst src/,build/,$(SRC_S:.S=.o))

LIB 		= build/libzeptos.a
EXEC 		= build/libzeptos

SIM 		:= spike --isa=rv$(XLEN)gc

all: $(EXEC) $(LIB)

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