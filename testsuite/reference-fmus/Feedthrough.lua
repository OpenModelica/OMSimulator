-- status: correct
-- teardown_command: rm -rf Feedthrough-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: yes
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Feedthrough-lua/")
oms_setWorkingDirectory("./Feedthrough-lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.Feedthrough", "../../resources/Feedthrough.fmu")

oms_setResultFile("model", "Feedthrough-cs.mat")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)

oms_instantiate("model")
oms_setBoolean("model.root.Feedthrough.Boolean_input", true)
oms_setInteger("model.root.Feedthrough.Enumeration_input", 2)
oms_setReal("model.root.Feedthrough.Float64_continuous_input", 3.1)
oms_setReal("model.root.Feedthrough.Float64_discrete_input", 4.2)
oms_setInteger("model.root.Feedthrough.Int32_input", 5)

oms_initialize("model")
oms_simulate("model")
print("info:    Feedthrough.Boolean_output: " .. oms_getBoolean("model.root.Feedthrough.Boolean_output"))
print("info:    Feedthrough.Enumeration_output: " .. oms_getInteger("model.root.Feedthrough.Enumeration_output"))
print("info:    Feedthrough.Float64_continuous_output: " .. oms_getReal("model.root.Feedthrough.Float64_continuous_output"))
print("info:    Feedthrough.Float64_discrete_output: " .. oms_getReal("model.root.Feedthrough.Float64_discrete_output"))
print("info:    Feedthrough.Int32_output: " .. oms_getInteger("model.root.Feedthrough.Int32_output"))

oms_terminate("model")
oms_delete("model")

oms_newModel("model")
oms_addSystem("model.root", oms_system_sc)

oms_addSubModel("model.root.Feedthrough", "../../resources/Feedthrough.fmu")

oms_setResultFile("model", "Feedthrough-me.mat")
oms_setStopTime("model", 10.0)
oms_setTolerance("model.root", 1e-5)

oms_instantiate("model")
oms_setBoolean("model.root.Feedthrough.Boolean_input", true)
oms_setInteger("model.root.Feedthrough.Enumeration_input", 2)
oms_setReal("model.root.Feedthrough.Float64_continuous_input", 3.1)
oms_setReal("model.root.Feedthrough.Float64_discrete_input", 4.2)
oms_setInteger("model.root.Feedthrough.Int32_input", 5)

oms_initialize("model")
oms_simulate("model")
print("info:    Feedthrough.Boolean_output: " .. oms_getBoolean("model.root.Feedthrough.Boolean_output"))
print("info:    Feedthrough.Enumeration_output: " .. oms_getInteger("model.root.Feedthrough.Enumeration_output"))
print("info:    Feedthrough.Float64_continuous_output: " .. oms_getReal("model.root.Feedthrough.Float64_continuous_output"))
print("info:    Feedthrough.Float64_discrete_output: " .. oms_getReal("model.root.Feedthrough.Float64_discrete_output"))
print("info:    Feedthrough.Int32_output: " .. oms_getInteger("model.root.Feedthrough.Int32_output"))

oms_terminate("model")
oms_delete("model")

-- Result:
-- info:    Result file: Feedthrough-cs.mat (bufferSize=1)
-- info:    Parameter model.root.Feedthrough.String_parameter will not be stored in the result file, because the signal type is not supported
-- info:    Feedthrough.Boolean_output: 1
-- info:    Feedthrough.Enumeration_output: 2
-- info:    Feedthrough.Float64_continuous_output: 3.1
-- info:    Feedthrough.Float64_discrete_output: 4.2
-- info:    Feedthrough.Int32_output: 5
-- info:    model doesn't contain any continuous state
-- info:    maximum step size for 'model.root': 0.001000
-- info:    Result file: Feedthrough-me.mat (bufferSize=1)
-- info:    Parameter model.root.Feedthrough.String_parameter will not be stored in the result file, because the signal type is not supported
-- info:    Feedthrough.Boolean_output: 1
-- info:    Feedthrough.Enumeration_output: 2
-- info:    Feedthrough.Float64_continuous_output: 3.1
-- info:    Feedthrough.Float64_discrete_output: 4.2
-- info:    Feedthrough.Int32_output: 5
-- info:    Final Statistics for 'model.root':
--          NumSteps = 10001 NumRhsEvals  = 10002 NumLinSolvSetups = 501
--          NumNonlinSolvIters = 10001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
