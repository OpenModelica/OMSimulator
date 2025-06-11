FMUs = \
A \
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
B \
BouncingBall \
Dahlquist \
DualMassOscillator.System1 \
DualMassOscillator.System2 \
DiscreteTest \
Enum1 \
equationPair.equation1 \
equationPair.equation2 \
EventTest \
Feedthrough \
fmi_attributes_19 \
fmidertest \
HelloWorld \
HelloWorldWithInput \
Int1 \
invalidModeldescription \
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
Modelica.Electrical.Analog.Examples.CauerLowPassAnalog \
nls.cs \
nls.me \
QuarterCarModel.DisplacementDisplacement.Chassis \
QuarterCarModel.DisplacementDisplacement.Wheel \
QuarterCarModel.DisplacementForce.Chassis \
QuarterCarModel.DisplacementForce.Wheel \
replaceA \
replaceA_extended \
replaceB \
Resource \
Stair \
str_hello_world \
tank1 \
tank2 \
tank3 \
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
testArray \
VanDerPol \
ypipe \

SSPs = \
embrace \
embrace_TwoConf \
import_hierarchical_ssv_sources \
importParameterMapping \
importParameterMappingInline \
importStartValues \
importStartValues1 \
import_export_parameters1 \
import_export_parameters2 \
import_export_parameters3 \
importExportAllResources \
invalidSRMD \
invalidSSD \
invalidSSV \
invalidSSM \
replaceSubmodel4 \
replaceSubmodel5 \
toplevelparameter1 \
toplevelparameter2 \

MOSs = \
ASSCExample.mos \
Blocks.mos \
DualMassOscillator.mos \
Enumeration.mos \
equationPair.mos \
EventTest.mos \
fmidertest.mos \
HelloWorld.mos \
HelloWorldWithInput.mos \
Lin2DimODE.mos \
Mass.mos \
MassSpring.mos \
nls.mos \
QuarterCarModel.mos \
replaceSubmodel.mos \
str_hello_world.mos \
unit.mos \

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
