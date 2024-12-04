-- status: correct
-- teardown_command: rm -rf VanDerPol-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./VanDerPol-lua/")
oms_setWorkingDirectory("./VanDerPol-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)
oms_addSubModel("model.root.VanDerPol", "../../resources/VanDerPol.fmu")
oms_setResultFile("model", "VanDerPol-cs.mat")
oms_setStopTime("model", 20.0)
oms_setTolerance("model.root", 1e-5)
oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
oms_terminate("model")
oms_delete("model")

if 1 == oms_compareSimulationResults("../../references/VanDerPol-cs.mat", "VanDerPol-cs.mat", "model.root.VanDerPol.x0", 1e-4, 1e-4) then
  print("signal x0 is equal")
else
  print("signal x0 is not equal")
end

if 1 == oms_compareSimulationResults("../../references/VanDerPol-cs.mat", "VanDerPol-cs.mat", "model.root.VanDerPol.x1", 1e-4, 1e-4) then
  print("signal x1 is equal")
else
  print("signal x1 is not equal")
end

oms_newModel("model")
oms_addSystem("model.root", oms_system_sc)
oms_addSubModel("model.root.VanDerPol", "../../resources/VanDerPol.fmu")
oms_setResultFile("model", "VanDerPol-me.mat")
oms_setStopTime("model", 20.0)
oms_setTolerance("model.root", 1e-5)
oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
oms_terminate("model")
oms_delete("model")

if 1 == oms_compareSimulationResults("../../references/VanDerPol-me.mat", "VanDerPol-me.mat", "model.root.VanDerPol.x0", 1e-4, 1e-4) then
  print("signal x0 is equal")
else
  print("signal x0 is not equal")
end

if 1 == oms_compareSimulationResults("../../references/VanDerPol-me.mat", "VanDerPol-me.mat", "model.root.VanDerPol.x1", 1e-4, 1e-4) then
  print("signal x1 is equal")
else
  print("signal x1 is not equal")
end

-- Result:
-- info:    Result file: VanDerPol-cs.mat (bufferSize=1)
-- signal x0 is equal
-- signal x1 is equal
-- info:    maximum step size for 'model.root': 0.001000
-- info:    Result file: VanDerPol-me.mat (bufferSize=1)
-- info:    Final Statistics for 'model.root':
--          NumSteps = 20002 NumRhsEvals  = 20004 NumLinSolvSetups = 1002
--          NumNonlinSolvIters = 20003 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- signal x0 is equal
-- signal x1 is equal
-- endResult
