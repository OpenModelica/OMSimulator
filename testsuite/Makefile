.PHONY: all test difftool resources partest

all: difftool resources test

test: api.log parker.log dcmotor.log simulation.log

partest: difftool resources
	cd partest && time ./runtests.pl -nocolour -with-xml

difftool:
	@$(MAKE) -C difftool

resources:
	@$(MAKE) -C resources

api.log: difftool
	@$(MAKE) -C api -f Makefile test > $@
	@grep == api.log

parker.log: difftool
	@$(MAKE) -C parker -f Makefile test > $@
	@grep == parker.log

dcmotor.log: difftool
	@$(MAKE) -C dcmotor -f Makefile test > $@
	@grep == dcmotor.log

simulation.log: difftool
	@$(MAKE) -C simulation -f Makefile test > $@
	@grep == simulation.log
