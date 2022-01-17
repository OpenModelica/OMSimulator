-- status: correct
-- teardown_command: rm -rf fmi_attribute_01_ME_lua/
-- linux: no
-- mingw32: no
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./fmi_attribute_01_ME_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_sc)

oms_addSubModel("model.root.fmi_attribute_01", "../resources/fmi_attributes_01_working.fmu")

oms_setResultFile("model", "fmi_attribute_01_ME.mat", 10)

oms_instantiate("model")

oms_initialize("model")

oms_simulate("model")

oms_terminate("model")
oms_delete("model")


-- Result:
-- info:    maximum step size for 'model.root': 0.001000
-- info:    Result file: fmi_attribute_01_ME.mat (bufferSize=10)
-- info:    Final Statistics for 'model.root':
--          NumSteps = 1001 NumRhsEvals  = 1002 NumLinSolvSetups = 51
--          NumNonlinSolvIters = 1001 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
