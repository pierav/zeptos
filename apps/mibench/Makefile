
BENCHS = $(shell ls -d */ | sed -s 's|/||')
$(info $(BENCHS))

EXECS = $(foreach b,$(BENCHS), build/$(b).riscv)
$(info $(EXECS))


%/build/exec: force
	make -C $*

build/%.riscv: %/build/exec
	@mkdir -p $(@D)
	cp $^ $@

run_all: $(EXECS)
	for test in $^; do echo $$test && spike -p1 $$test; done

.phony: force
force:
	@: