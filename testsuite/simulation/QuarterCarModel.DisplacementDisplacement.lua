-- status: correct
-- teardown_command: rm -rf QuarterCarModel/ QuarterCarModel.DisplacementDisplacement.mat
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true --solverStats=true")
oms_setTempDirectory("./QuarterCarModel/")

oms_newModel("QuarterCarModel")
oms_addSystem("QuarterCarModel.DisplacementDisplacement", oms_system_wc)

-- instantiate FMUs
oms_addSubModel("QuarterCarModel.DisplacementDisplacement.Chassis", "../resources/QuarterCarModel.DisplacementDisplacement.Chassis.fmu")
oms_addSubModel("QuarterCarModel.DisplacementDisplacement.Wheel", "../resources/QuarterCarModel.DisplacementDisplacement.Wheel.fmu")
oms_addSubModel("QuarterCarModel.DisplacementDisplacement.BC", "../resources/road.csv")

-- add connections
oms_addConnection("QuarterCarModel.DisplacementDisplacement.Chassis.z_c", "QuarterCarModel.DisplacementDisplacement.Wheel.z_c")
oms_addConnection("QuarterCarModel.DisplacementDisplacement.Chassis.der_z_c", "QuarterCarModel.DisplacementDisplacement.Wheel.der_z_c")
oms_addConnection("QuarterCarModel.DisplacementDisplacement.Chassis.z_w", "QuarterCarModel.DisplacementDisplacement.Wheel.z_w")
oms_addConnection("QuarterCarModel.DisplacementDisplacement.Chassis.der_z_w", "QuarterCarModel.DisplacementDisplacement.Wheel.der_z_w")
oms_addConnection("QuarterCarModel.DisplacementDisplacement.BC.road", "QuarterCarModel.DisplacementDisplacement.Wheel.z")

-- simulation settings
oms_setResultFile("QuarterCarModel", "QuarterCarModel.DisplacementDisplacement.mat", 100)
oms_setStopTime("QuarterCarModel", 1.0)
--oms_setSolver("QuarterCarModel", oms_solver_wc_mav)
--oms_setVariableStepSize("QuarterCarModel.DisplacementDisplacement", 1e-2, 1e-2, 1e-2)
oms_setFixedStepSize("QuarterCarModel.DisplacementDisplacement", 1e-3)
oms_setTolerance("QuarterCarModel.DisplacementDisplacement", 1e-4, 1e-4)

oms_instantiate("QuarterCarModel")
oms_initialize("QuarterCarModel")
oms_simulate("QuarterCarModel")
oms_terminate("QuarterCarModel")
oms_delete("QuarterCarModel")

-- Result:
-- info:    Result file: QuarterCarModel.DisplacementDisplacement.mat (bufferSize=100)
-- endResult
