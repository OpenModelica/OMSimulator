-- status: correct
-- teardown_command: rm -rf simple_loop-lua/ simple_loop_res.mat
-- linux: no
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./simple_loop-lua/")


oms_newModel("model")

oms_addSystem("model.root", oms_system_sc)

oms_addSubModel("model.root.A", "../resources/simpleLoop.fmu")

oms_setVariableStepSize("model", 1e-6, 1e-12, 0.002);
oms_setResultFile("model", "simple_loop_res.mat")

oms_instantiate("model")

oms_initialize("model")

oms_simulate("model")

oms_terminate("model")

oms_delete("model")


-- Result:
-- info:    maximum step size for 'model.root': 0.002000
-- info:    Result file: simple_loop_res.mat (bufferSize=1)
-- info:    Final Statistics for 'model.root':
--          NumSteps = 501 NumRhsEvals  = 502 NumLinSolvSetups = 27
--          NumNonlinSolvIters = 501 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
