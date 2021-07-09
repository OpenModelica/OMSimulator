## status: correct
## linux: yes
## mingw32: yes
## mingw64: yes
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
oms.setReal("test.Root.Gain.k", 27)

oms.addSubModel("test.Root.add", "../resources/Modelica.Blocks.Math.Add.fmu")
oms.setReal("test.Root.add.k1", -20)

oms.exportSSVTemplate("test", "template1.ssv")
readFile("template1.ssv")

oms.exportSSVTemplate("test.Root.add", "add1.ssv")
readFile("add1.ssv")

oms.exportSSVTemplate("test.Root.Gain", "gain1.ssv")
readFile("gain1.ssv")


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
