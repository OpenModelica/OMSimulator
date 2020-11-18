## status: correct
## linux: yes
## mingw: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./exportssvtemplate_py/")


def readFile(filename):
  f = open(filename, "r")
  content = f.read()
  print(content)
  f:close()



oms.newModel("test")

oms.addSystem("test.Root", oms.system_wc)

oms.addSubModel("test.Root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms.addSubModel("test.Root.add", "../resources/Modelica.Blocks.Math.Add.fmu")

oms.exportSSVTemplate("test", "template.ssv")
readFile("template.ssv")

oms.exportSSVTemplate("test.Root.add", "add.ssv")
readFile("add.ssv")

oms.exportSSVTemplate("test.Root.Gain", "gain.ssv")
readFile("gain.ssv")


## Result:
## <?xml version="1.0" encoding="UTF-8"?>
## <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="1.0" name="modelDescriptionStartValues">
## 	<ssv:Parameters>
## 		<ssv:Parameter name="add.u2">
## 			<ssv:Real value="0" />
## 		</ssv:Parameter>
## 		<ssv:Parameter name="add.u1">
## 			<ssv:Real value="0" />
## 		</ssv:Parameter>
## 		<ssv:Parameter name="add.k2">
## 			<ssv:Real value="1" />
## 		</ssv:Parameter>
## 		<ssv:Parameter name="add.k1">
## 			<ssv:Real value="1" />
## 		</ssv:Parameter>
## 		<ssv:Parameter name="Gain.u">
## 			<ssv:Real value="0" />
## 		</ssv:Parameter>
## 		<ssv:Parameter name="Gain.k">
## 			<ssv:Real value="1" />
## 		</ssv:Parameter>
## 	</ssv:Parameters>
## </ssv:ParameterSet>
##
## <?xml version="1.0" encoding="UTF-8"?>
## <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="1.0" name="modelDescriptionStartValues">
## 	<ssv:Parameters>
## 		<ssv:Parameter name="add.u2">
## 			<ssv:Real value="0" />
## 		</ssv:Parameter>
## 		<ssv:Parameter name="add.u1">
## 			<ssv:Real value="0" />
## 		</ssv:Parameter>
## 		<ssv:Parameter name="add.k2">
## 			<ssv:Real value="1" />
## 		</ssv:Parameter>
## 		<ssv:Parameter name="add.k1">
## 			<ssv:Real value="1" />
## 		</ssv:Parameter>
## 	</ssv:Parameters>
## </ssv:ParameterSet>
##
## <?xml version="1.0" encoding="UTF-8"?>
## <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="1.0" name="modelDescriptionStartValues">
## 	<ssv:Parameters>
## 		<ssv:Parameter name="Gain.u">
## 			<ssv:Real value="0" />
## 		</ssv:Parameter>
## 		<ssv:Parameter name="Gain.k">
## 			<ssv:Real value="1" />
## 		</ssv:Parameter>
## 	</ssv:Parameters>
## </ssv:ParameterSet>
##
## endResult
