-- status: correct
-- teardown_command: rm -rf Resource3-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Resource3-lua/")
oms_setWorkingDirectory("./Resource3-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)
oms_addSubModel("model.root.Resource", "../../../resources/Resource3.fmu")
oms_setResultFile("model", "Resource-cs.mat")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)
oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")
print("info:    Resource.y: " .. oms_getInteger("model.root.Resource.y"))
oms_terminate("model")
oms_delete("model")

-- Result:
-- Loading FMI version 3...
-- info:    Result file: Resource-cs.mat (bufferSize=1)
-- info:    Resource.y: 97
-- endResult
