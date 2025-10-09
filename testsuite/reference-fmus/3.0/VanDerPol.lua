-- status: correct
-- teardown_command: rm -rf VanDerPol3-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./VanDerPol3-lua/")
oms_setWorkingDirectory("./VanDerPol3-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)
oms_addSubModel("model.root.VanDerPol", "../../../resources/VanDerPol3.fmu")
oms_setResultFile("model", "VanDerPol-cs.mat")
oms_setStopTime("model", 20.0)
oms_setTolerance("model.root", 1e-5)
oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
oms_terminate("model")
oms_delete("model")

if 1 == oms_compareSimulationResults("../../../references/VanDerPol-cs.mat", "VanDerPol-cs.mat", "model.root.VanDerPol.x0", 1e-4, 1e-4) then
  print("signal x0 is equal")
else
  print("signal x0 is not equal")
end

if 1 == oms_compareSimulationResults("../../../references/VanDerPol-cs.mat", "VanDerPol-cs.mat", "model.root.VanDerPol.x1", 1e-4, 1e-4) then
  print("signal x1 is equal")
else
  print("signal x1 is not equal")
end

-- Result:
-- Loading FMI version 3...
-- info:    Result file: VanDerPol-cs.mat (bufferSize=1)
-- signal x0 is equal
-- signal x1 is equal
-- endResult
