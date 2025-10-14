-- status: correct
-- teardown_command: rm -rf connectFmi2AndFmi3-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./connectFmi2AndFmi3-lua/")
oms_setWorkingDirectory("./connectFmi2AndFmi3-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

--FMI2.0
oms_addSubModel("model.root.Gain1", "../../resources/Modelica.Blocks.Math.Gain.fmu")

--FMI3.0
oms_addSubModel("model.root.Feedthrough1", "../../resources/Feedthrough3.fmu")

--connections (Gain.y (FMI2) --> Feedthrough.Float64_continuous_input (FMI3))
oms_addConnection("model.root.Gain1.y", "model.root.Feedthrough1.Float64_continuous_input")

oms_setResultFile("model", "")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)

oms_setReal("model.root.Gain1.u", 15)

oms_instantiate("model")

oms_initialize("model")
oms_simulate("model")

print("info:    model.root.Gain1.u : " .. oms_getReal("model.root.Gain1.u"))
print("info:    model.root.Gain1.y : " .. oms_getReal("model.root.Gain1.y"))

print("info:    Feedthrough1.Float64_continuous_input  : " .. oms_getReal("model.root.Feedthrough1.Float64_continuous_input"))
print("info:    Feedthrough1.Float64_continuous_output : " .. oms_getReal("model.root.Feedthrough1.Float64_continuous_output"))

oms_terminate("model")
oms_delete("model")

-- Result:
-- warning: Unknown FMI3 base type for var : Binary_input
-- warning: Unknown FMI3 base type for var : Binary_output
-- Loading FMI version 3...
-- info:    No result file will be created
-- info:    model.root.Gain1.u : 15.0
-- info:    model.root.Gain1.y : 15.0
-- info:    Feedthrough1.Float64_continuous_input  : 15.0
-- info:    Feedthrough1.Float64_continuous_output : 15.0
-- info:    2 warnings
-- info:    0 errors
-- endResult
