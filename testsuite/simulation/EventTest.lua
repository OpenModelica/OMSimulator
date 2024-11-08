-- status: correct
-- teardown_command: rm -rf EventTest_lua/
-- linux: yes
-- ucrt64: no
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./EventTest_lua/")
oms_setWorkingDirectory("./EventTest_lua/")

oms_newModel("EventTest")
oms_addSystem("EventTest.root", oms_system_sc)
oms_addSubModel("EventTest.root.model", "../../resources/EventTest.fmu")

-- simulation settings
oms_setResultFile("EventTest", "EventTest_lua.csv")
oms_setStopTime("EventTest", 2.0)
oms_setFixedStepSize("EventTest.root", 1.0)

oms_instantiate("EventTest")
oms_setReal("EventTest.root.model.start_height", 0.3)
oms_initialize("EventTest")
oms_simulate("EventTest")
oms_terminate("EventTest")
oms_delete("EventTest")

-- Result:
-- info:    maximum step size for 'EventTest.root': 1.000000
-- info:    Result file: EventTest_lua.csv (bufferSize=1)
-- info:    Final Statistics for 'EventTest.root':
--          NumSteps = 1 NumRhsEvals  = 2 NumLinSolvSetups = 1
--          NumNonlinSolvIters = 1 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
