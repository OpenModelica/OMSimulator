-- status: correct
-- teardown_command: rm -rf Resource-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Resource-lua/")
oms_setWorkingDirectory("./Resource-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)
oms_addSubModel("model.root.Resource", "../../resources/Resource.fmu")
oms_setResultFile("model", "Resource-cs.mat")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)
oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
print("info:    Resource.y: " .. oms_getInteger("model.root.Resource.y"))
oms_terminate("model")
oms_delete("model")

oms_newModel("model")
oms_addSystem("model.root", oms_system_sc)
oms_addSubModel("model.root.Resource", "../../resources/Resource.fmu")
oms_setResultFile("model", "Resource-me.mat")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)
oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
print("info:    Resource.y: " .. oms_getInteger("model.root.Resource.y"))
oms_terminate("model")
oms_delete("model")

-- Result:
-- info:    Result file: Resource-cs.mat (bufferSize=1)
-- info:    Resource.y: 97
-- info:    model doesn't contain any continuous state
-- info:    maximum step size for 'model.root': 0.001000
-- info:    Result file: Resource-me.mat (bufferSize=1)
-- info:    Resource.y: 97
-- info:    Final Statistics for 'model.root':
--          NumSteps = 10001 NumRhsEvals  = 10002 NumLinSolvSetups = 501
--          NumNonlinSolvIters = 10001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
