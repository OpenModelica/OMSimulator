-- status: correct
-- teardown_command: rm -rf Absolute_clock-lua/ Absolute_clock_res.mat
-- linux: no
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Absolute_clock-lua/")


oms_newModel("model")

oms_addSystem("model.root", oms_system_sc)

oms_addSubModel("model.root.A", "../resources/Modelica.Clocked.Examples.CascadeControlledDrive.AbsoluteClocks.fmu")

oms_setVariableStepSize("model", 1e-6, 1e-12, 0.002);
oms_setResultFile("model", "Absolute_clock_res.mat")

oms_instantiate("model")

oms_initialize("model")

oms_simulate("model")

oms_terminate("model")

oms_delete("model")


-- Result:
-- info:    maximum step size for 'model.root': 0.002000
-- info:    Result file: Absolute_clock_res.mat (bufferSize=1)
-- info:    Parameter model.root.A.fastClock.solverMethod will not be stored in the result file, because the signal type is not supported
-- info:    Parameter model.root.A.slowClock.solverMethod will not be stored in the result file, because the signal type is not supported
-- info:    Final Statistics for 'model.root':
--          NumSteps = 2 NumRhsEvals  = 3 NumLinSolvSetups = 2
--          NumNonlinSolvIters = 2 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
