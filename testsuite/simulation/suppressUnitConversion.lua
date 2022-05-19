-- status: correct
-- teardown_command: rm -rf suppressunit_lua/
-- linux: no
-- mingw32: no
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./suppressunit_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.A", "../resources/A.fmu")
oms_addSubModel("model.root.B", "../resources/B.fmu")

-- invalid connection with different unit
oms_addConnection("model.root.A.y", "model.root.B.u")
-- valid connection mm to m
oms_addConnection("model.root.A.y1", "model.root.B.u1", true)
oms_addConnection("model.root.A.y2", "model.root.B.u2", true)

oms_setResultFile("model", "suppressunits.mat")

src, status = oms_exportSnapshot("model")
-- print(src)

oms_instantiate("model")
oms_initialize("model")
print("info:    Initialization")
print("info:      model.root.A.y1     : " .. oms_getReal("model.root.A.y1"))
print("info:      model.root.B.u1     : " .. oms_getReal("model.root.B.u1"))
print("info:      model.root.A.y2     : " .. oms_getReal("model.root.A.y2"))
print("info:      model.root.B.u2     : " .. oms_getReal("model.root.B.u2"))

oms_simulate("model")
print("info:    Simulation")
print("info:      model.root.A.y1     : " .. oms_getReal("model.root.A.y1"))
print("info:      model.root.B.u1     : " .. oms_getReal("model.root.B.u1"))
print("info:      model.root.A.y2     : " .. oms_getReal("model.root.A.y2"))
print("info:      model.root.B.u2     : " .. oms_getReal("model.root.B.u2"))

oms_terminate("model")
oms_delete("model")



-- Result:
-- error:   [addConnection] Unit mismatch in connection: A.y -> B.u
-- info:    Result file: suppressunits.mat (bufferSize=1)
-- info:    Initialization
-- info:      model.root.A.y1     : 5.0
-- info:      model.root.B.u1     : 5.0
-- info:      model.root.A.y2     : 5.0
-- info:      model.root.B.u2     : 5.0
-- info:    Simulation
-- info:      model.root.A.y1     : 5.0
-- info:      model.root.B.u1     : 5.0
-- info:      model.root.A.y2     : 5.0
-- info:      model.root.B.u2     : 5.0
-- info:    0 warnings
-- info:    1 errors
-- endResult
