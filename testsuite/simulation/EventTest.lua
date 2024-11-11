-- status: correct
-- teardown_command: rm -rf EventTest_lua/
-- linux: yes
-- ucrt64: no
-- win: no
-- mac: no

function readFile(filename)
  local f = assert(io.open(filename, "r"))
  local content = f:read("*all")
  print(content)
  f:close()
end

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./EventTest_lua/")
oms_setWorkingDirectory("./EventTest_lua/")

oms_newModel("EventTest")
oms_addSystem("EventTest.root", oms_system_sc)
oms_addSubModel("EventTest.root.model", "../../resources/EventTest.fmu")

-- simulation settings
oms_setSolver("EventTest.root", oms_solver_sc_explicit_euler)
--oms_setSolver("EventTest.root", oms_solver_sc_cvode)
oms_setResultFile("EventTest", "EventTest_lua.csv")
oms_setStopTime("EventTest", 2.0)
oms_setFixedStepSize("EventTest.root", 1.0)

oms_instantiate("EventTest")
oms_setReal("EventTest.root.model.start_height", 0.3)
oms_initialize("EventTest")
oms_simulate("EventTest")
oms_terminate("EventTest")
oms_delete("EventTest")

readFile("EventTest_lua.csv")

-- Result:
-- info:    Result file: EventTest_lua.csv (bufferSize=1)
-- time,EventTest.root.model.height,EventTest.root.model.der(height)
-- 0, 0.3, -1
-- 0.300018310547, -1.8310546875e-05, -1
-- 0.300018310547, 0.3, -1
-- 0.600036621094, -1.8310546875e-05, -1
-- 0.600036621094, 0.3, -1
-- 0.900085449219, -4.8828125e-05, -1
-- 0.900085449219, 0.3, -1
-- 1, 0.200085449219, -1
-- 1.20013427734, -4.8828125e-05, -1
-- 1.20013427734, 0.3, -1
-- 1.50018310547, -4.8828125e-05, -1
-- 1.50018310547, 0.3, -1
-- 1.80023193359, -4.8828125e-05, -1
-- 1.80023193359, 0.3, -1
-- 2, 0.100231933594, -1
--
-- endResult
