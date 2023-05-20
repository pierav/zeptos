
ZETPOS_PATH = $(CURDIR)/../../..
ZEPTOS_RAMFS_PATH = $(CURDIR)/ramfs

include $(ZETPOS_PATH)/include.mk

CFLAGS 		= $(ZEPTOS_CFLAGS)
LDFLAGS		= $(ZEPTOS_LDFLAGS)

SRC 		= $(wildcard src/*.c)
OBJ 		= $(addprefix build/,$(SRC:.c=.o))
INCS 		= -I/$(ZETPOS_PATH)/include 

EXEC = build/exec

all: $(EXEC)

build/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

$(EXEC): $(OBJ) $(ZEPTOS_LIB) $(ZEPTOS_RAMFS_OBJ)
	@mkdir -p $(@D)
	$(CC) $(ZEPTOS_CFLAGS) -o $@ $^  $(ZEPTOS_LDFLAGS)

.phony: run_all