.PHONY: all test difftool resources partest

all: difftool resources test

test: api.log AircraftVehicleDemonstrator.log import.log simulation.log tlm.log assc.log

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

simulation.log: difftool
	@$(MAKE) -C simulation -f Makefile test > $@
	@grep == simulation.log

tlm.log: difftool
	@$(MAKE) -C tlm -f Makefile test > $@
	@grep == tlm.log

assc.log: difftool
	@$(MAKE) -C assc -f Makefile test > $@
	@grep == assc.log
