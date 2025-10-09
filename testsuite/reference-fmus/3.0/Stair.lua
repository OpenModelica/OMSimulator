-- status: correct
-- teardown_command: rm -rf Stair-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Stair3-lua/")
oms_setWorkingDirectory("./Stair3-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.Stair", "../../../resources/Stair3.fmu")

oms_setResultFile("model", "Stair-cs.mat")
oms_setStopTime("model", 9)
oms_setTolerance("model.root", 1e-5)
oms_setFixedStepSize("model.root", 0.2)

oms_instantiate("model")

oms_initialize("model")
oms_simulate("model")
oms_terminate("model")
oms_delete("model")

if 1 == oms_compareSimulationResults("../../../references/Stair-cs.mat", "Stair-cs.mat", "model.root.Stair.counter", 1e-4, 1e-4) then
  print("signal counter is equal")
else
  print("signal counter is not equal")
end

-- Result:
-- Loading FMI version 3...
-- info:    Result file: Stair-cs.mat (bufferSize=1)
-- signal counter is equal
-- endResult
