## status: correct
## teardown_command: rm -rf duplicateactivate_02_py/
## linux: yes
## mingw32: no
## mingw64: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./duplicateactivate_02_py/")

oms.newModel("model")

oms.addSystem("model.root", oms.system_wc)

oms.addSubModel("model.root.A", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms.setReal("model.root.A.k", 10)

oms.setResultFile("model", "duplicateactivate1.mat")

print("info:    Current Variant SystemStructure.ssd")
print("info:      model.root.A.u      : " , oms.getReal("model.root.A.u")[0])
print("info:      model.root.A.k      : " , oms.getReal("model.root.A.k")[0])

oms.duplicateVariant("model", "varA")
oms.setReal("varA.root.A.u", -10)

print("info:    Activate Variant varA")
print("info:      varA.root.A.u      : " , oms.getReal("varA.root.A.u")[0])
print("info:      varA.root.A.k      : " , oms.getReal("varA.root.A.k")[0])

oms.duplicateVariant("varA", "varB")
oms.setReal("varB.root.A.u", -13)
oms.setReal("varB.root.A.k", -100)

print("info:    Activate Variant varB")
print("info:      varB.root.A.u      : " , oms.getReal("varB.root.A.u")[0])
print("info:      varB.root.A.k      : " , oms.getReal("varB.root.A.k")[0])

## activate varB to varA
oms.activateVariant("varB", "varA")
print("info:    Reactivate Variant varB to varA ")
print("info:      varA.root.A.u      : " , oms.getReal("varA.root.A.u")[0])
print("info:      varA.root.A.k      : " , oms.getReal("varA.root.A.k")[0])

## activate varA to model
oms.activateVariant("varA", "model")
print("info:    Reactivate Variant varA to model ")
print("info:      model.root.A.u      : " , oms.getReal("model.root.A.u")[0])
print("info:      model.root.A.k      : " , oms.getReal("model.root.A.k")[0])

oms.export("model", "duplicateactivate1.ssp")

oms.terminate("model")
oms.delete("model")

oms.importFile("duplicateactivate1.ssp")
variants, status = oms.listVariants("model")
print(variants)

src, status = oms.exportSnapshot("model")
print(src)

oms.activateVariant("model", "varA")
src, status = oms.exportSnapshot("varA")
print(src)

oms.activateVariant("varA", "varB")
src, status = oms.exportSnapshot("varB")
print(src)

oms.terminate("varB")
oms.delete("varB")

## Result:
## info:    Current Variant SystemStructure.ssd
## info:      model.root.A.u      :  0.0
## info:      model.root.A.k      :  10.0
## info:    Activate Variant varA
## info:      varA.root.A.u      :  -10.0
## info:      varA.root.A.k      :  10.0
## info:    Activate Variant varB
## info:      varB.root.A.u      :  -13.0
## info:      varB.root.A.k      :  -100.0
## info:    Reactivate Variant varB to varA
## info:      varA.root.A.u      :  -10.0
## info:      varA.root.A.k      :  10.0
## info:    Reactivate Variant varA to model
## info:      model.root.A.u      :  0.0
## info:      model.root.A.k      :  10.0
## <?xml version="1.0"?>
## <oms:snapshot
##   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
##   partial="false">
##   <oms:file
##     name="ssdVariants.xml">
##     <oms:Variants
##       version="1.0">
##       <oms:variant
##         name="model" />
##       <oms:variant
##         name="varB" />
##       <oms:variant
##         name="varA" />
##     </oms:Variants>
##   </oms:file>
## </oms:snapshot>
##
## <?xml version="1.0"?>
## <oms:snapshot
##   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
##   partial="false">
##   <oms:file
##     name="SystemStructure.ssd">
##     <ssd:SystemStructureDescription
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping"
##       xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary"
##       xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
##       name="model"
##       version="1.0">
##       <ssd:System
##         name="root">
##         <ssd:Elements>
##           <ssd:Component
##             name="A"
##             type="application/x-fmu-sharedlibrary"
##             source="resources/0001_A.fmu">
##             <ssd:Connectors>
##               <ssd:Connector
##                 name="u"
##                 kind="input">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="0.000000"
##                   y="0.500000" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="y"
##                 kind="output">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="1.000000"
##                   y="0.500000" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="k"
##                 kind="parameter">
##                 <ssc:Real
##                   unit="1" />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:ParameterBindings>
##               <ssd:ParameterBinding>
##                 <ssd:ParameterValues>
##                   <ssv:ParameterSet
##                     version="1.0"
##                     name="parameters">
##                     <ssv:Parameters>
##                       <ssv:Parameter
##                         name="k">
##                         <ssv:Real
##                           value="10"
##                           unit="1" />
##                       </ssv:Parameter>
##                     </ssv:Parameters>
##                   </ssv:ParameterSet>
##                 </ssd:ParameterValues>
##               </ssd:ParameterBinding>
##             </ssd:ParameterBindings>
##           </ssd:Component>
##         </ssd:Elements>
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation>
##                 <oms:FixedStepMaster
##                   description="oms-ma"
##                   stepSize="0.001000"
##                   absoluteTolerance="0.000100"
##                   relativeTolerance="0.000100" />
##               </oms:SimulationInformation>
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:System>
##       <ssd:Units>
##         <ssc:Unit
##           name="1">
##           <ssc:BaseUnit />
##         </ssc:Unit>
##       </ssd:Units>
##       <ssd:DefaultExperiment
##         startTime="0.000000"
##         stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation
##                 resultFile="duplicateactivate1.mat"
##                 loggingInterval="0.000000"
##                 bufferSize="1"
##                 signalFilter="resources/signalFilter.xml" />
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:DefaultExperiment>
##     </ssd:SystemStructureDescription>
##   </oms:file>
##   <oms:file
##     name="resources/signalFilter.xml">
##     <oms:SignalFilter
##       version="1.0">
##       <oms:Variable
##         name="model.root.A.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="model.root.A.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.A.k"
##         type="Real"
##         kind="parameter" />
##     </oms:SignalFilter>
##   </oms:file>
## </oms:snapshot>
##
## <?xml version="1.0"?>
## <oms:snapshot
##   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
##   partial="false">
##   <oms:file
##     name="varA.ssd">
##     <ssd:SystemStructureDescription
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping"
##       xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary"
##       xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
##       name="varA"
##       version="1.0">
##       <ssd:System
##         name="root">
##         <ssd:Elements>
##           <ssd:Component
##             name="A"
##             type="application/x-fmu-sharedlibrary"
##             source="resources/0001_A.fmu">
##             <ssd:Connectors>
##               <ssd:Connector
##                 name="u"
##                 kind="input">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="0.000000"
##                   y="0.500000" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="y"
##                 kind="output">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="1.000000"
##                   y="0.500000" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="k"
##                 kind="parameter">
##                 <ssc:Real
##                   unit="1" />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:ParameterBindings>
##               <ssd:ParameterBinding>
##                 <ssd:ParameterValues>
##                   <ssv:ParameterSet
##                     version="1.0"
##                     name="parameters">
##                     <ssv:Parameters>
##                       <ssv:Parameter
##                         name="u">
##                         <ssv:Real
##                           value="-10" />
##                       </ssv:Parameter>
##                       <ssv:Parameter
##                         name="k">
##                         <ssv:Real
##                           value="10"
##                           unit="1" />
##                       </ssv:Parameter>
##                     </ssv:Parameters>
##                   </ssv:ParameterSet>
##                 </ssd:ParameterValues>
##               </ssd:ParameterBinding>
##             </ssd:ParameterBindings>
##           </ssd:Component>
##         </ssd:Elements>
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation>
##                 <oms:FixedStepMaster
##                   description="oms-ma"
##                   stepSize="0.001000"
##                   absoluteTolerance="0.000100"
##                   relativeTolerance="0.000100" />
##               </oms:SimulationInformation>
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:System>
##       <ssd:Units>
##         <ssc:Unit
##           name="1">
##           <ssc:BaseUnit />
##         </ssc:Unit>
##       </ssd:Units>
##       <ssd:DefaultExperiment
##         startTime="0.000000"
##         stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation
##                 resultFile="duplicateactivate1.mat"
##                 loggingInterval="0.000000"
##                 bufferSize="1"
##                 signalFilter="resources/signalFilter_varA.xml" />
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:DefaultExperiment>
##     </ssd:SystemStructureDescription>
##   </oms:file>
##   <oms:file
##     name="resources/signalFilter_varA.xml">
##     <oms:SignalFilter
##       version="1.0">
##       <oms:Variable
##         name="varA.root.A.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="varA.root.A.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="varA.root.A.k"
##         type="Real"
##         kind="parameter" />
##     </oms:SignalFilter>
##   </oms:file>
## </oms:snapshot>
##
## <?xml version="1.0"?>
## <oms:snapshot
##   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
##   partial="false">
##   <oms:file
##     name="varB.ssd">
##     <ssd:SystemStructureDescription
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping"
##       xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary"
##       xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
##       name="varB"
##       version="1.0">
##       <ssd:System
##         name="root">
##         <ssd:Elements>
##           <ssd:Component
##             name="A"
##             type="application/x-fmu-sharedlibrary"
##             source="resources/0001_A.fmu">
##             <ssd:Connectors>
##               <ssd:Connector
##                 name="u"
##                 kind="input">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="0.000000"
##                   y="0.500000" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="y"
##                 kind="output">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="1.000000"
##                   y="0.500000" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="k"
##                 kind="parameter">
##                 <ssc:Real
##                   unit="1" />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:ParameterBindings>
##               <ssd:ParameterBinding>
##                 <ssd:ParameterValues>
##                   <ssv:ParameterSet
##                     version="1.0"
##                     name="parameters">
##                     <ssv:Parameters>
##                       <ssv:Parameter
##                         name="u">
##                         <ssv:Real
##                           value="-13" />
##                       </ssv:Parameter>
##                       <ssv:Parameter
##                         name="k">
##                         <ssv:Real
##                           value="-100"
##                           unit="1" />
##                       </ssv:Parameter>
##                     </ssv:Parameters>
##                   </ssv:ParameterSet>
##                 </ssd:ParameterValues>
##               </ssd:ParameterBinding>
##             </ssd:ParameterBindings>
##           </ssd:Component>
##         </ssd:Elements>
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation>
##                 <oms:FixedStepMaster
##                   description="oms-ma"
##                   stepSize="0.001000"
##                   absoluteTolerance="0.000100"
##                   relativeTolerance="0.000100" />
##               </oms:SimulationInformation>
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:System>
##       <ssd:Units>
##         <ssc:Unit
##           name="1">
##           <ssc:BaseUnit />
##         </ssc:Unit>
##       </ssd:Units>
##       <ssd:DefaultExperiment
##         startTime="0.000000"
##         stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation
##                 resultFile="duplicateactivate1.mat"
##                 loggingInterval="0.000000"
##                 bufferSize="1"
##                 signalFilter="resources/signalFilter_varB.xml" />
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:DefaultExperiment>
##     </ssd:SystemStructureDescription>
##   </oms:file>
##   <oms:file
##     name="resources/signalFilter_varB.xml">
##     <oms:SignalFilter
##       version="1.0">
##       <oms:Variable
##         name="varB.root.A.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="varB.root.A.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="varB.root.A.k"
##         type="Real"
##         kind="parameter" />
##     </oms:SignalFilter>
##   </oms:file>
## </oms:snapshot>
##
## endResult
