## status: correct
## teardown_command: rm -rf replacesubmodel_14_py/
## linux: no
## mingw32: no
## mingw64: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./replacesubmodel_14_py/")
oms.setWorkingDirectory("./replacesubmodel_14_py/")

oms.newModel("model")

oms.addSystem("model.root", oms.system_wc)

oms.addSubModel("model.root.A", "../../resources/replaceA.fmu")

print("info:    Before replacing the Model")
print("info:      model.root.A.u      : " , oms.getReal("model.root.A.u")[0])

oms.replaceSubModel("model.root.A", "../../resources/replaceA_extended.fmu", False)

## no priority exists read the new value from modeldescription.xml from the replaced component
print("info:    After replacing the Model")
print("info:      model.root.A.u      : " , oms.getReal("model.root.A.u")[0])

oms.terminate("model")
oms.delete("model")

## Result:
## info:    Before replacing the Model
## info:      model.root.A.u      :  5.0
## info:    After replacing the Model
## info:      model.root.A.u      :  7.0
## endResult
