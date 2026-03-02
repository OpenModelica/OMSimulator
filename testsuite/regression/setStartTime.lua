-- status: correct
-- teardown_command: rm -rf start_time-lua/ start_time_res.mat
-- linux: no
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./start_time-lua/")


oms_newModel("model")

oms_addSystem("model.root", oms_system_sc)

oms_addSubModel("model.root.A", "../resources/setstartTime.fmu")

oms_setStartTime("model", 2.5)
oms_setStopTime("model", 5.0)
oms_setVariableStepSize("model", 1e-6, 1e-12, 0.002);
oms_setResultFile("model", "start_time_res.mat")

oms_instantiate("model")

oms_initialize("model")

oms_simulate("model")

oms_terminate("model")

oms_delete("model")


-- Result:
-- info:    maximum step size for 'model.root': 0.002000
-- info:    Result file: start_time_res.mat (bufferSize=1)
-- info:    Parameter model.root.A.scheme.fileName will not be stored in the result file, because the signal type is not supported
-- info:    Parameter model.root.A.scheme.tableName will not be stored in the result file, because the signal type is not supported
-- info:    Final Statistics for 'model.root':
--          NumSteps = 1251 NumRhsEvals  = 1252 NumLinSolvSetups = 65
--          NumNonlinSolvIters = 1251 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
