## status: correct
## linux: yes
## mingw: no
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

oms.exportSSMTemplate("test", "template.ssm")
readFile("template.ssm")

oms.exportSSMTemplate("test.Root.add", "add.ssm")
readFile("add.ssm")

oms.exportSSMTemplate("test.Root.Gain", "gain.ssm")
readFile("gain.ssm")


## Result:
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
## endResult
