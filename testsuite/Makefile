.PHONY: all test difftool resources partest

all: difftool resources test

test: api.log AircraftVehicleDemonstrator.log OMSimulator.log OMSysIdent.log tlm.log

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

OMSimulator.log: difftool
	@$(MAKE) -C OMSimulator -f Makefile test > $@
	@grep == OMSimulator.log

OMSysIdent.log: difftool
	@$(MAKE) -C OMSysIdent -f Makefile test > $@
	@grep == OMSysIdent.log

tlm.log: difftool
	@$(MAKE) -C tlm -f Makefile test > $@
	@grep == tlm.log
