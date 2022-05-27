-- status: correct
-- teardown_command: rm -rf tankypipe_lua/
-- linux: no
-- mingw32: no
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./tankypipe_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.tank1", "../resources/tank1.fmu")
oms_addSubModel("model.root.tank2", "../resources/tank2.fmu")
oms_addSubModel("model.root.tank3", "../resources/tank3.fmu")
oms_addSubModel("model.root.ypipe", "../resources/ypipe.fmu")

oms_addConnection("model.root.tank1.outputRate", "model.root.ypipe.inputRate1")
oms_addConnection("model.root.tank2.outputRate", "model.root.ypipe.inputRate2")
oms_addConnection("model.root.ypipe.outputRate", "model.root.tank3.inputRate")


oms_setResultFile("model", "tankypipe_res.mat")

oms_setStopTime("model", 1.0) -- set the stopTime = 100 to match the testing with EDF
oms_setSolver("model", oms_solver_wc_mav)

-- oms_setVariableStepSize("model",  0.01, 1e-6, 1e-6)  -- settings to check for rollbacks

-- src, status = oms_exportSnapshot("model")
-- print(src)

oms_instantiate("model")
oms_initialize("model")
oms_simulate("model")

oms_terminate("model")
oms_delete("model")


-- Result:
-- info:    Result file: tankypipe_res.mat (bufferSize=1)
-- endResult
