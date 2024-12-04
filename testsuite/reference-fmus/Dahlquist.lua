-- status: correct
-- teardown_command: rm -rf Dahlquist-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Dahlquist-lua/")
oms_setWorkingDirectory("./Dahlquist-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.Dahlquist", "../../resources/Dahlquist.fmu")

oms_setResultFile("model", "Dahlquist-cs.mat")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)

oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
oms_terminate("model")
oms_delete("model")

if 1 == oms_compareSimulationResults("../../references/Dahlquist-cs.mat", "Dahlquist-cs.mat", "model.root.Dahlquist.x", 1e-4, 1e-4) then
  print("signal x is equal")
else
  print("signal x is not equal")
end

oms_newModel("model")
oms_addSystem("model.root", oms_system_sc)

oms_addSubModel("model.root.Dahlquist", "../../resources/Dahlquist.fmu")

oms_setResultFile("model", "Dahlquist-me.mat")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)

oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
oms_terminate("model")
oms_delete("model")

if 1 == oms_compareSimulationResults("../../references/Dahlquist-me.mat", "Dahlquist-me.mat", "model.root.Dahlquist.x", 1e-4, 1e-4) then
  print("signal x is equal")
else
  print("signal x is not equal")
end

-- Result:
-- info:    Result file: Dahlquist-cs.mat (bufferSize=1)
-- signal x is equal
-- info:    maximum step size for 'model.root': 0.001000
-- info:    Result file: Dahlquist-me.mat (bufferSize=1)
-- info:    Final Statistics for 'model.root':
--          NumSteps = 10001 NumRhsEvals  = 10002 NumLinSolvSetups = 501
--          NumNonlinSolvIters = 10001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- signal x is equal
-- endResult
