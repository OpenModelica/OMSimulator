-- status: correct
-- teardown_command: rm -rf BouncingBall3-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./BouncingBall3-lua/")
oms_setWorkingDirectory("./BouncingBall3-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.BouncingBall", "../../../resources/BouncingBall3.fmu")

oms_setResultFile("model", "BouncingBall-cs.mat")
oms_setStopTime("model", 3.0)
oms_setTolerance("model.root", 1e-5)

oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
oms_terminate("model")
oms_delete("model")

if 1 == oms_compareSimulationResults("../../../references/BouncingBall-cs.mat", "BouncingBall-cs.mat", "model.root.BouncingBall.h", "model.root.BouncingBall.h", 1e-4, 1e-4) then
  print("signal h is equal")
else
  print("signal h is not equal")
end

if 1 == oms_compareSimulationResults("../../../references/BouncingBall-cs.mat", "BouncingBall-cs.mat", "model.root.BouncingBall.v", "model.root.BouncingBall.v", 1e-4, 1e-4) then
  print("signal v is equal")
else
  print("signal v is not equal")
end

-- Result:
-- Loading FMI version 3...
-- info:    Result file: BouncingBall-cs.mat (bufferSize=1)
-- signal h is equal
-- signal v is equal
-- endResult
