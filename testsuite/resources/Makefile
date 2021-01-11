FMUs = \
AircraftVehicleDemonstrator.Atmosphere_Model_FMU_sf \
AircraftVehicleDemonstrator.bC \
AircraftVehicleDemonstrator.cockpit \
AircraftVehicleDemonstrator.consumer_A \
AircraftVehicleDemonstrator.consumer_B \
AircraftVehicleDemonstrator.eCS_Generic_Export \
AircraftVehicleDemonstrator.eCS_SW \
AircraftVehicleDemonstrator.engine \
ASSCExample.Class1 \
ASSCExample.Class2 \
DualMassOscillator.System1 \
DualMassOscillator.System2 \
Enum1 \
HelloWorld \
HelloWorldWithInput \
Int1 \
Lin2DimODE \
Modelica.Blocks.Continuous.Integrator \
Modelica.Blocks.Math.Add \
Modelica.Blocks.Math.Add3 \
Modelica.Blocks.Math.Feedback \
Modelica.Blocks.Math.Gain \
Modelica.Blocks.Math.Product \
Modelica.Blocks.Nonlinear.Limiter \
Modelica.Blocks.Sources.Clock \
Modelica.Blocks.Sources.Constant \
Modelica.Blocks.Sources.Sine \
Modelica.Blocks.Sources.Step \
QuarterCarModel.DisplacementDisplacement.Chassis \
QuarterCarModel.DisplacementDisplacement.Wheel \
QuarterCarModel.DisplacementForce.Chassis \
QuarterCarModel.DisplacementForce.Wheel \
tlm.adder \
tlm.CoarseGrained1_1D \
tlm.CoarseGrained2_1D \
tlm.FineGrained1_1D \
tlm.FineGrained2_1D \
tlm.gain \
tlm.LowerPendulum \
tlm.LowerPendulumFG \
tlm.mass \
tlm.massSpring \
tlm.source \
tlm.UpperPendulum \
tlm.UpperPendulumFG \
tlm.UpperPendulumFG \

SSPs = \
importStartValues \
importParameterMapping \
importParameterMappingInline \
import_hierarchical_ssv_sources \


MOSs = \
ASSCExample.mos \
DualMassOscillator.mos \
Enumeration.mos \
HelloWorldWithInput.mos \
Mass.mos \
QuarterCarModel.mos \
Blocks.mos \
HelloWorld.mos \
Lin2DimODE.mos \
MassSpring.mos \

.PHONY: all generateFMUs 

all:
	@echo
	@echo preparing FMUs...
	@for fmu in $(FMUs) ; do cd $$fmu && zip -r ../$$fmu.fmu * && cd .. ; done
	@for ssp in $(SSPs) ; do cd $$ssp && zip -r ../$$ssp.ssp * && cd .. ; done


generateFMUs:
	@echo
	@echo generating FMUs with $$(omc --version) from $$(which omc)
	@for mos in $(MOSs) ; do omc $$mos && rm *.libs *.log *.makefile; done
