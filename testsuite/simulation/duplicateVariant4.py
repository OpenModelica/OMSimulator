## status: correct
## teardown_command: rm -rf duplicatevariant_04_py/
## linux: yes
## mingw32: no
## mingw64: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./duplicatevariant_04_py/")

oms.newModel("model")

oms.addSystem("model.root", oms.system_wc)

oms.addSubModel("model.root.A", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms.setReal("model.root.A.k", 10)

oms.setResultFile("model", "duplicatevariant4.mat")

## export SystemStructure.ssd
src, status = oms.exportSnapshot("model")
print(src)

oms.duplicateVariant("model", "varA")
oms.setReal("varA.root.A.u", -10)

## export varA.ssd
src, status = oms.exportSnapshot("varA")
print(src)

oms.duplicateVariant("varA", "varB")
oms.setReal("varB.root.A.u", -13)
oms.setReal("varB.root.A.k", -100)


## export varB.ssd
src, status = oms.exportSnapshot("varB")
print(src)

oms.export("varB", "multiVariant4.ssp")

oms.terminate("varB")
oms.delete("varB")


## Result:
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
##       version="2.0">
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
##                     version="2.0"
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
##                 resultFile="duplicatevariant4.mat"
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
##       version="2.0">
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
##       version="2.0">
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
##                     version="2.0"
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
##                 resultFile="duplicatevariant4.mat"
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
##       version="2.0">
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
##       version="2.0">
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
##                     version="2.0"
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
##                 resultFile="duplicatevariant4.mat"
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
##       version="2.0">
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
