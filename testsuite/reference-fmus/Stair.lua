-- status: correct
-- teardown_command: rm -rf Stair-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Stair-lua/")
oms_setWorkingDirectory("./Stair-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.Stair", "../../resources/Stair.fmu")

oms_setResultFile("model", "Stair-cs.mat")
oms_setStopTime("model", 10)
oms_setTolerance("model.root", 1e-5)
oms_setFixedStepSize("model.root", 0.2)

oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
oms_terminate("model")
oms_delete("model")

if 1 == oms_compareSimulationResults("../../references/Stair-cs.mat", "Stair-cs.mat", "model.root.Stair.counter", 1e-4, 1e-4) then
  print("signal counter is equal")
else
  print("signal counter is not equal")
end

oms_newModel("model")
oms_addSystem("model.root", oms_system_sc)
oms_addSubModel("model.root.Stair", "../../resources/Stair.fmu")
oms_setResultFile("model", "Stair-me.mat")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)
oms_setFixedStepSize("model.root", 0.2)
oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
oms_terminate("model")
oms_delete("model")

if 1 == oms_compareSimulationResults("../../references/Stair-me.mat", "Stair-me.mat", "model.root.Stair.counter", 1e-4, 1e-4) then
  print("signal counter is equal")
else
  print("signal counter is not equal")
end

-- Result:
-- info:    Result file: Stair-cs.mat (bufferSize=1)
-- info:    fmi2_doStep discarded for FMU "model.root.Stair"
-- signal counter is equal
-- info:    model doesn't contain any continuous state
-- info:    maximum step size for 'model.root': 0.200000
-- info:    Result file: Stair-me.mat (bufferSize=1)
-- info:    Simulation terminated by FMU model.root.Stair at time 9.000000
-- info:    Final Statistics for 'model.root':
--          NumSteps = 0 NumRhsEvals  = 0 NumLinSolvSetups = 0
--          NumNonlinSolvIters = 0 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- signal counter is equal
-- endResult
