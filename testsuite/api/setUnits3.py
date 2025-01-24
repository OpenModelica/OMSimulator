## status: correct
## teardown_command: rm -rf setunits_03_py/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./setunits_03_py/")

oms.newModel("model")
oms.addSystem("model.root", oms.system_wc)
oms.addSubModel("model.root.sine", "../resources/Modelica.Blocks.Sources.Sine.fmu")

## add resources to submodule
oms.newResources("model.root.sine:sine.ssv")

oms.setReal("model.root.sine.phase", 27)
oms.setReal("model.root.sine.amplitude", -100)
oms.setReal("model.root.sine.f", -300)

oms.setResultFile("model", "")

(src, status) = oms.exportSnapshot("model:resources/sine.ssv")
print(src)

## change the unit to meter
oms.setUnit("model.root.sine.phase", "m")
oms.setUnit("model.root.sine.amplitude", "m")

(src, status) = oms.exportSnapshot("model:resources/sine.ssv")
print(src)

oms.terminate("model")
oms.delete("model")



## Result:
## <?xml version="1.0"?>
## <oms:snapshot
##   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
##   partial="true">
##   <oms:file
##     name="resources/sine.ssv">
##     <ssv:ParameterSet
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       version="2.0"
##       name="parameters">
##       <ssv:Parameters>
##         <ssv:Parameter
##           name="phase">
##           <ssv:Real
##             value="27"
##             unit="rad" />
##         </ssv:Parameter>
##         <ssv:Parameter
##           name="f">
##           <ssv:Real
##             value="-300"
##             unit="Hz" />
##         </ssv:Parameter>
##         <ssv:Parameter
##           name="amplitude">
##           <ssv:Real
##             value="-100" />
##         </ssv:Parameter>
##       </ssv:Parameters>
##       <ssv:Units>
##         <ssc:Unit
##           name="rad">
##           <ssc:BaseUnit />
##         </ssc:Unit>
##         <ssc:Unit
##           name="Hz">
##           <ssc:BaseUnit
##             s="-1" />
##         </ssc:Unit>
##       </ssv:Units>
##     </ssv:ParameterSet>
##   </oms:file>
## </oms:snapshot>
##
## <?xml version="1.0"?>
## <oms:snapshot
##   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
##   partial="true">
##   <oms:file
##     name="resources/sine.ssv">
##     <ssv:ParameterSet
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       version="2.0"
##       name="parameters">
##       <ssv:Parameters>
##         <ssv:Parameter
##           name="phase">
##           <ssv:Real
##             value="27"
##             unit="m" />
##         </ssv:Parameter>
##         <ssv:Parameter
##           name="f">
##           <ssv:Real
##             value="-300"
##             unit="Hz" />
##         </ssv:Parameter>
##         <ssv:Parameter
##           name="amplitude">
##           <ssv:Real
##             value="-100"
##             unit="m" />
##         </ssv:Parameter>
##       </ssv:Parameters>
##       <ssv:Units>
##         <ssc:Unit
##           name="m">
##           <ssc:BaseUnit />
##         </ssc:Unit>
##         <ssc:Unit
##           name="Hz">
##           <ssc:BaseUnit
##             s="-1" />
##         </ssc:Unit>
##       </ssv:Units>
##     </ssv:ParameterSet>
##   </oms:file>
## </oms:snapshot>
##
## endResult
