-- status: correct
-- teardown_command: rm -rf fmuState_lua/
-- linux: no
-- mac: no
-- mingw32: yes
-- mingw64: yes
-- win: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./fmuState_lua/")

oms_newModel("fmuState")

oms_addSystem("fmuState.root", oms_system_wc)

oms_addSubModel("fmuState.root.A", "../resources/fmi_attributes_19.fmu")

oms_setStopTime("fmuState", 5.0)

oms_setResultFile("fmuState", "fmustate01_res.mat")

oms_instantiate("fmuState")
oms_initialize("fmuState")

print("info: Initial Settings")
print("info:      fmuState.root.A.rp1: " .. oms_getReal("fmuState.root.A.rp1"))
print("info:      fmuState.root.A.rp2: " .. oms_getInteger("fmuState.root.A.rp2"))

oms_stepUntil("fmuState", 1.0)

oms_setReal("fmuState.root.A.rp1", -10)
oms_setInteger("fmuState.root.A.rp2", -20)

print("info: New Start values")
print("info:      fmuState.root.A.rp1 at time 1.0: " .. oms_getReal("fmuState.root.A.rp1"))
print("info:      fmuState.root.A.rp2 at time 1.0: " .. oms_getInteger("fmuState.root.A.rp2"))

oms_stepUntil("fmuState", 2.0)

oms_getState("fmuState.root.A")
oms_setState("fmuState.root.A")
oms_setState("fmuState.root.A")

print("info: Rollback")
print("info:      fmuState.root.A.rp1 at time 2.0: " .. oms_getReal("fmuState.root.A.rp1"))
print("info:      fmuState.root.A.rp2 at time 2.0: " .. oms_getInteger("fmuState.root.A.rp2"))

oms_terminate("fmuState")
oms_delete("fmuState")


-- Result:
-- info:    Result file: fmustate01_res.mat (bufferSize=1)
-- info: Initial Settings
-- info:      fmuState.root.A.rp1: 6.0
-- info:      fmuState.root.A.rp2: 7
-- info: New Start values
-- info:      fmuState.root.A.rp1 at time 1.0: -10.0
-- info:      fmuState.root.A.rp2 at time 1.0: -20
-- info: Rollback
-- info:      fmuState.root.A.rp1 at time 2.0: 6.0
-- info:      fmuState.root.A.rp2 at time 2.0: 7
-- endResult
