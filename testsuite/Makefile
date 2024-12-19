.PHONY: all test difftool resources partest

all: difftool resources test

test: api.log AircraftVehicleDemonstrator.log import.log export.log simulation.log reference-fmus.log

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

import.log: difftool
	@$(MAKE) -C import -f Makefile test > $@
	@grep == import.log

export.log: export
	@$(MAKE) -C export -f Makefile test > $@
	@grep == export.log

simulation.log: difftool
	@$(MAKE) -C simulation -f Makefile test > $@
	@grep == simulation.log

reference-fmus.log: difftool
	@$(MAKE) -C reference-fmus -f Makefile test > $@
	@grep == reference-fmus.log

reference-fmus3.log: difftool
	@$(MAKE) -C reference-fmus/3.0 -f Makefile test > $@
	@grep == reference-fmus3.log