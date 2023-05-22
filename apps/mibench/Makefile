
BENCHS := $(shell ls -d */ | sed -s 's|/||')
BENCHS := $(filter-out build, $(BENCHS))
$(info $(BENCHS))

EXECS = $(foreach b,$(BENCHS), build/$(b).riscv)
$(info $(EXECS))


%/build/exec: force
	make -C $*

build/%.riscv: %/build/exec
	@mkdir -p $(@D)
	ln -s ../$^ $@

run_all: $(EXECS)
	for test in $^; do echo $$test && time spike -p1 $$test; done

.phony: force
force:
	@:
