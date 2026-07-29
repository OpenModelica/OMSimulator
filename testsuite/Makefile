.PHONY: all test difftool resources partest

all: difftool resources test

test: api.log parker.log AircraftVehicleDemonstrator.log CompositeModels.log dcmotor.log simulation.log reference-fmus-2-cs.log reference-fmus-2-me.log reference-fmus-3-cs.log reference-fmus-3-me.log

partest: difftool resources
	cd partest && time ./runtests.pl -nocolour -with-xml

difftool:
	@$(MAKE) -C difftool

resources:
	@$(MAKE) -C resources

api.log: difftool
	@$(MAKE) -C api -f Makefile test > $@
	@grep == api.log

AircraftVehicleDemonstrator.log: difftool
	@$(MAKE) -C AircraftVehicleDemonstrator -f Makefile test > $@
	@grep == AircraftVehicleDemonstrator.log

CompositeModels.log: difftool
	@$(MAKE) -C CompositeModels -f Makefile test > $@
	@grep == CompositeModels.log

parker.log: difftool
	@$(MAKE) -C parker -f Makefile test > $@
	@grep == parker.log

dcmotor.log: difftool
	@$(MAKE) -C dcmotor -f Makefile test > $@
	@grep == dcmotor.log

simulation.log: difftool
	@$(MAKE) -C simulation -f Makefile test > $@
	@grep == simulation.log

reference-fmus-2-cs.log: difftool
	@$(MAKE) -C reference-fmus/2.0/cs -f Makefile test > $@
	@grep == reference-fmus-2-cs.log

reference-fmus-2-me.log: difftool
	@$(MAKE) -C reference-fmus/2.0/me -f Makefile test > $@
	@grep == reference-fmus-2-me.log

reference-fmus-3-cs.log: difftool
	@$(MAKE) -C reference-fmus/3.0/cs -f Makefile test > $@
	@grep == reference-fmus-3-cs.log

reference-fmus-3-me.log: difftool
	@$(MAKE) -C reference-fmus/3.0/me -f Makefile test > $@
	@grep == reference-fmus-3-me.log
