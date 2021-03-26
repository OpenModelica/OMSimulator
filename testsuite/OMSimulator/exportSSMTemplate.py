## status: correct
## teardown_command: rm -rf exportssmtemplate_py/ add2.ssm gain2.ssm template2.ssm
## linux: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./exportssmtemplate_py/")


def readFile(filename):
  f = open(filename, "r")
  content = f.read()
  print(content)
  f:close()



oms.newModel("test")

oms.addSystem("test.Root", oms.system_wc)

oms.addSubModel("test.Root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms.addSubModel("test.Root.add", "../resources/Modelica.Blocks.Math.Add.fmu")

oms.exportSSMTemplate("test", "template2.ssm")
readFile("template2.ssm")

oms.exportSSMTemplate("test.Root.add", "add2.ssm")
readFile("add2.ssm")

oms.exportSSMTemplate("test.Root.Gain", "gain2.ssm")
readFile("gain2.ssm")


## Result:
## warning: [Gain: resources/0001_Gain.fmu] The FMU lists 0 initial unknowns but actually contains 1 initial unknowns as per the variable definitions.
## info:    [Gain: resources/0001_Gain.fmu] The FMU contains bad initial unknowns. This might cause problems, e.g. wrong simulation results.
## <?xml version="1.0" encoding="UTF-8"?>
## <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="1.0">
## 	<ssm:MappingEntry source="" target="add.u2" />
## 	<ssm:MappingEntry source="" target="add.u1" />
## 	<ssm:MappingEntry source="" target="add.k2" />
## 	<ssm:MappingEntry source="" target="add.k1" />
## 	<ssm:MappingEntry source="" target="Gain.u" />
## 	<ssm:MappingEntry source="" target="Gain.k" />
## </ssm:ParameterMapping>
##
## <?xml version="1.0" encoding="UTF-8"?>
## <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="1.0">
## 	<ssm:MappingEntry source="" target="add.u2" />
## 	<ssm:MappingEntry source="" target="add.u1" />
## 	<ssm:MappingEntry source="" target="add.k2" />
## 	<ssm:MappingEntry source="" target="add.k1" />
## </ssm:ParameterMapping>
##
## <?xml version="1.0" encoding="UTF-8"?>
## <ssm:ParameterMapping xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" version="1.0">
## 	<ssm:MappingEntry source="" target="Gain.u" />
## 	<ssm:MappingEntry source="" target="Gain.k" />
## </ssm:ParameterMapping>
##
## info:    1 warnings
## info:    0 errors
## endResult
