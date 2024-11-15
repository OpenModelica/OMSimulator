## status: correct
## teardown_command: rm -rf fmuState_py/
## linux: no
## mac: no
## mingw32: yes
## mingw64: yes
## win: no
## asan: yes

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./fmuState_py/")

oms.newModel("fmuState")

oms.addSystem("fmuState.root", oms.system_wc)

oms.addSubModel("fmuState.root.A", "../resources/fmi_attributes_19.fmu")

oms.setStopTime("fmuState", 5.0)

oms.setResultFile("fmuState", "fmustate02_res.mat")

oms.instantiate("fmuState")
oms.initialize("fmuState")

print("info: Initial Settings")
print("info:      fmuState.root.A.rp1: " , oms.getReal("fmuState.root.A.rp1")[0])
print("info:      fmuState.root.A.rp2: " , oms.getInteger("fmuState.root.A.rp2")[0])

oms.stepUntil("fmuState", 1.0)

oms.setReal("fmuState.root.A.rp1", -10)
oms.setInteger("fmuState.root.A.rp2", -20)

print("info: New Start values")
print("info:      fmuState.root.A.rp1 at time 1.0: " , oms.getReal("fmuState.root.A.rp1")[0])
print("info:      fmuState.root.A.rp2 at time 1.0: " , oms.getInteger("fmuState.root.A.rp2")[0])

oms.stepUntil("fmuState", 2.0)

oms.getState("fmuState.root.A")
oms.setState("fmuState.root.A")
oms.freeState("fmuState.root.A")

print("info: Rollback")
print("info:      fmuState.root.A.rp1 at time 2.0: " , oms.getReal("fmuState.root.A.rp1")[0])
print("info:      fmuState.root.A.rp2 at time 2.0: " , oms.getInteger("fmuState.root.A.rp2")[0])

oms.terminate("fmuState")
oms.delete("fmuState")


## Result:
## info:    Result file: fmustate02_res.mat (bufferSize=1)
## info: Initial Settings
## info:      fmuState.root.A.rp1:  6.0
## info:      fmuState.root.A.rp2:  7
## info: New Start values
## info:      fmuState.root.A.rp1 at time 1.0:  -10.0
## info:      fmuState.root.A.rp2 at time 1.0:  -20
## info: Rollback
## info:      fmuState.root.A.rp1 at time 2.0:  6.0
## info:      fmuState.root.A.rp2 at time 2.0:  7
## endResult
