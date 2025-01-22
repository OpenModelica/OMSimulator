-- status: correct
-- teardown_command: rm -rf FeedthroughConnections3-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./FeedthroughConnections3-lua/")
oms_setWorkingDirectory("./FeedthroughConnections3-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.Feedthrough1", "../../resources/Feedthrough3.fmu")
oms_addSubModel("model.root.Feedthrough2", "../../resources/Feedthrough3.fmu")

--connections
oms_addConnection("model.root.Feedthrough1.Float64_continuous_output", "model.root.Feedthrough2.Float64_continuous_input")
oms_addConnection("model.root.Feedthrough1.Int64_output", "model.root.Feedthrough2.Int64_input")

oms_setResultFile("model", "")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)

oms_setReal("model.root.Feedthrough1.Float64_continuous_input", 3.5)
oms_setInteger("model.root.Feedthrough1.Int64_input", 7)

oms_instantiate("model")

oms_initialize("model")
oms_simulate("model")

print("info:    Feedthrough1.Float64_continuous_input  : " .. oms_getReal("model.root.Feedthrough1.Float64_continuous_input"))
print("info:    Feedthrough1.Float64_continuous_output : " .. oms_getReal("model.root.Feedthrough1.Float64_continuous_output"))

print("info:    Feedthrough2.Float64_continuous_input  : " .. oms_getReal("model.root.Feedthrough2.Float64_continuous_input"))
print("info:    Feedthrough2.Float64_continuous_output : " .. oms_getReal("model.root.Feedthrough2.Float64_continuous_output"))

print("info:    Feedthrough1.Int64_input               : " .. oms_getInteger("model.root.Feedthrough1.Int64_input"))
print("info:    Feedthrough1.Int64_output              : " .. oms_getInteger("model.root.Feedthrough1.Int64_output"))

print("info:    Feedthrough2.Int64_input               : " .. oms_getInteger("model.root.Feedthrough2.Int64_input"))
print("info:    Feedthrough2.Int64_output              : " .. oms_getInteger("model.root.Feedthrough2.Int64_output"))

oms_terminate("model")
oms_delete("model")

-- Result:
-- warning: Unknown FMI3 base type for var : Binary_input
-- warning: Unknown FMI3 base type for var : Binary_output
-- warning: Unknown FMI3 base type for var : Binary_input
-- warning: Unknown FMI3 base type for var : Binary_output
-- Loading FMI version 3...
-- Loading FMI version 3...
-- info:    No result file will be created
-- info:    Feedthrough1.Float64_continuous_input  : 3.5
-- info:    Feedthrough1.Float64_continuous_output : 3.5
-- info:    Feedthrough2.Float64_continuous_input  : 3.5
-- info:    Feedthrough2.Float64_continuous_output : 3.5
-- info:    Feedthrough1.Int64_input               : 7
-- info:    Feedthrough1.Int64_output              : 7
-- info:    Feedthrough2.Int64_input               : 7
-- info:    Feedthrough2.Int64_output              : 7
-- info:    4 warnings
-- info:    0 errors
-- endResult
