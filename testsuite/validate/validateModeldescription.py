## status: correct
## teardown_command: rm -rf invalidmodeldescription-py/
## linux: yes
## mac: no
## mingw32: no
## mingw64: yes
## win: yes

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./invalidmodeldescription-py/")
oms.setWorkingDirectory("./invalidmodeldescription-py/")

oms.newModel("model")
oms.addSystem("model.root", oms.system_wc)

oms.addSubModel("model.root.add", "../../resources/invalidModeldescription.fmu")


## Result:
## warning: invalid "modeldescription.xml" detected in file "../../resources/invalidModeldescription.fmu" at line: 188 column: 6, value 'xxxx' does not match regular expression facet '[+\-]?[0-9]+'
## warning: "modeldescription.xml" does not conform to the FMI-2.0 standard schema
## info:    2 warnings
## info:    0 errors
## endResult
