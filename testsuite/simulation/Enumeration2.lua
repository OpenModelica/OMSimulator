-- status: correct
-- teardown_command: rm -rf Enumeration2_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./Enumeration2_lua/")
oms_setWorkingDirectory("./Enumeration2_lua/")

oms_newModel("test")
oms_addSystem("test.root", oms_system_sc)
oms_addSubModel("test.root.A", "../../resources/Int1.fmu")
oms_addSubModel("test.root.B", "../../resources/Enum1.fmu")

-- valid connection
oms_addConnection("test.root.B.u", "test.root.A.y")

oms_setResultFile("test", "enumeration2.mat")

oms_instantiate("test")
print("info:    Instantiation")
print("info:      test.root.A.y     : " .. oms_getInteger("test.root.A.y"))
print("info:      test.root.B.u     : " .. oms_getInteger("test.root.B.u"))

oms_initialize("test")
print("info:    Initialization")
print("info:      test.root.A.y     : " .. oms_getInteger("test.root.A.y"))
print("info:      test.root.B.u     : " .. oms_getInteger("test.root.B.u"))

oms_simulate("test")
print("info:    Simulation")
print("info:      test.root.A.y     : " .. oms_getInteger("test.root.A.y"))
print("info:      test.root.B.u     : " .. oms_getInteger("test.root.B.u"))

oms_terminate("test")
oms_delete("test")



-- Result:
-- info:    model doesn't contain any continuous state
-- info:    Instantiation
-- info:      test.root.A.y     : 2
-- info:      test.root.B.u     : 1
-- info:    Result file: enumeration2.mat (bufferSize=1)
-- info:    Initialization
-- info:      test.root.A.y     : 2
-- info:      test.root.B.u     : 2
-- info:    Simulation
-- info:      test.root.A.y     : 2
-- info:      test.root.B.u     : 2
-- endResult
