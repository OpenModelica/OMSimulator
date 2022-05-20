-- status: correct
-- teardown_command: rm -rf fmi_attributes_lua/
-- linux: no
-- mingw32: no
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./fmi_attributes_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.A", "../resources/fmi_attributes_19.fmu")

oms_setResultFile("model", "fmi_attributes.mat")

oms_setStopTime("model", 10.0)
oms_setSolver("model", oms_solver_wc_mav)


oms_instantiate("model")
oms_initialize("model")
print("info:    Initialization")
print("info:      model.root.A.x1      : " .. oms_getReal("model.root.A.x1"))
print("info:      model.root.A.rp1     : " .. oms_getReal("model.root.A.rp1"))
print("info:      model.root.A.rp2     : " .. oms_getInteger("model.root.A.rp2"))
print("info:      model.root.A.rp3     : " .. oms_getBoolean("model.root.A.rp3"))

oms_simulate("model")
print("info:    Simulation")
print("info:      model.root.A.x1      : " .. oms_getReal("model.root.A.x1"))
print("info:      model.root.A.rp1     : " .. oms_getReal("model.root.A.rp1"))
print("info:      model.root.A.rp2     : " .. oms_getInteger("model.root.A.rp2"))
print("info:      model.root.A.rp3     : " .. oms_getBoolean("model.root.A.rp3"))

oms_terminate("model")
oms_delete("model")



-- Result:
-- info:    Result file: fmi_attributes.mat (bufferSize=1)
-- info:    Initialization
-- info:      model.root.A.x1      : 3.0
-- info:      model.root.A.rp1     : 6.0
-- info:      model.root.A.rp2     : 7
-- info:      model.root.A.rp3     : 1
-- info:    Simulation
-- info:      model.root.A.x1      : 3.0
-- info:      model.root.A.rp1     : 6.0
-- info:      model.root.A.rp2     : 7
-- info:      model.root.A.rp3     : 1
-- endResult
