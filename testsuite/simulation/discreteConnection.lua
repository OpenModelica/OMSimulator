-- status: correct
-- teardown_command: rm -rf discreteconnections-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true  --algLoopSolver=fixedpoint")
oms_setTempDirectory("./discreteconnections-lua/")
oms_setWorkingDirectory("./discreteconnections-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.discreteconnection1", "../../resources/DiscreteTest.fmu")
oms_addSubModel("model.root.discreteconnection2", "../../resources/DiscreteTest.fmu")

oms_addConnection("model.root.discreteconnection1.in_real", "model.root.discreteconnection2.out_real")
oms_addConnection("model.root.discreteconnection1.in_bool", "model.root.discreteconnection2.out_bool")
--oms_addConnection("model.root.discreteconnection1.out_bool", "model.root.discreteconnection2.in_bool")
oms_addConnection("model.root.discreteconnection1.out_real", "model.root.discreteconnection2.in_real")

oms_setResultFile("model", "discreteconnection-cs.mat")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5, 1e-5)

oms_instantiate("model")

oms_setBoolean("model.root.discreteconnection1.in_bool", true)
oms_setReal("model.root.discreteconnection1.in_real", 3.1)
oms_setReal("model.root.discreteconnection2.in_real", 3.1)

oms_initialize("model")
oms_simulate("model")

oms_terminate("model")
oms_delete("model")

-- Result:
-- info:    Alg. loop (size 3/7)
--            model.root.discreteconnection2
--            model.root.discreteconnection1
-- info:    Result file: discreteconnection-cs.mat (bufferSize=1)
-- endResult
