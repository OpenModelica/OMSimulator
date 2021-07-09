## status: correct
## teardown_command: rm -rf exportSSVTemplate_py/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: yes

import OMSimulator as oms

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./exportSSVTemplate_py/")
oms.setWorkingDirectory("./exportSSVTemplate_py/")

model = oms.newModel("exportSSVTemplate")
root = model.addSystem("root", oms.Types.System.WC)

root.addSubModel("Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")
root.setReal("Gain.k", 27)

root.addSubModel("add", "../../resources/Modelica.Blocks.Math.Add.fmu")
root.setReal("add.k1", -20)

model.exportSSVTemplate("", "template.ssv")
with open("template.ssv") as f:
  print(f.read())

model.exportSSVTemplate("root.add", "add.ssv")
with open("add.ssv") as f:
  print(f.read())

model.exportSSVTemplate("root.Gain", "gain.ssv")
with open("gain.ssv") as f:
  print(f.read())

model.delete()

## Result:
## <?xml version="1.0" encoding="UTF-8"?>
## <ssv:ParameterSet
##   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##   version="1.0"
##   name="modelDescriptionStartValues">
##   <ssv:Parameters>
##     <ssv:Parameter
##       name="add.u2">
##       <ssv:Real
##         value="0" />
##     </ssv:Parameter>
##     <ssv:Parameter
##       name="add.u1">
##       <ssv:Real
##         value="0" />
##     </ssv:Parameter>
##     <ssv:Parameter
##       name="add.k2">
##       <ssv:Real
##         value="1" />
##     </ssv:Parameter>
##     <ssv:Parameter
##       name="add.k1">
##       <ssv:Real
##         value="-20" />
##     </ssv:Parameter>
##     <ssv:Parameter
##       name="Gain.u">
##       <ssv:Real
##         value="0" />
##     </ssv:Parameter>
##     <ssv:Parameter
##       name="Gain.k">
##       <ssv:Real
##         value="27" />
##     </ssv:Parameter>
##   </ssv:Parameters>
## </ssv:ParameterSet>
##
## <?xml version="1.0" encoding="UTF-8"?>
## <ssv:ParameterSet
##   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##   version="1.0"
##   name="modelDescriptionStartValues">
##   <ssv:Parameters>
##     <ssv:Parameter
##       name="add.u2">
##       <ssv:Real
##         value="0" />
##     </ssv:Parameter>
##     <ssv:Parameter
##       name="add.u1">
##       <ssv:Real
##         value="0" />
##     </ssv:Parameter>
##     <ssv:Parameter
##       name="add.k2">
##       <ssv:Real
##         value="1" />
##     </ssv:Parameter>
##     <ssv:Parameter
##       name="add.k1">
##       <ssv:Real
##         value="-20" />
##     </ssv:Parameter>
##   </ssv:Parameters>
## </ssv:ParameterSet>
##
## <?xml version="1.0" encoding="UTF-8"?>
## <ssv:ParameterSet
##   xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##   xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##   version="1.0"
##   name="modelDescriptionStartValues">
##   <ssv:Parameters>
##     <ssv:Parameter
##       name="Gain.u">
##       <ssv:Real
##         value="0" />
##     </ssv:Parameter>
##     <ssv:Parameter
##       name="Gain.k">
##       <ssv:Real
##         value="27" />
##     </ssv:Parameter>
##   </ssv:Parameters>
## </ssv:ParameterSet>
##
## endResult
