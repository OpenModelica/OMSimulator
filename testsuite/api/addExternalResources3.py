## status: correct
## teardown_command: rm -rf addExternalResources3_py/
## linux: yes
## mingw32: yes
## mingw64: yes
## win: yes
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./addExternalResources3_py/")
oms.setWorkingDirectory("./addExternalResources3_py/")

oms.newModel("addExternalResources")

oms.addSystem("addExternalResources.root", oms.system_wc)
oms.addConnector("addExternalResources.root.Input1", oms.input, oms.signal_type_real)
oms.addConnector("addExternalResources.root.Input2", oms.input, oms.signal_type_real)

## add Top level resources
oms.newResources("addExternalResources.root:root.ssv")

oms.setReal("addExternalResources.root.Input1", 10)
oms.setReal("addExternalResources.root.Input2", 50)

oms.addSystem("addExternalResources.root.system1", oms.system_sc)
oms.addConnector("addExternalResources.root.system1.C1", oms.input, oms.signal_type_real)
oms.addConnector("addExternalResources.root.system1.C2", oms.input, oms.signal_type_real)

## add resources to subsystem
oms.newResources("addExternalResources.root.system1:system1.ssv")
oms.setReal("addExternalResources.root.system1.C1", -10)

oms.addSubModel("addExternalResources.root.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

## add resources to submodule
oms.newResources("addExternalResources.root.Gain:gain.ssv")

oms.setReal("addExternalResources.root.Gain.k", 27)

oms.setResultFile("addExternalResources", "addExternalResources1.mat", 10)

oms.export("addExternalResources", "addExternalResources3.ssp")

oms.terminate("addExternalResources")
oms.delete("addExternalResources")

oms.importFile("addExternalResources3.ssp")

## old Resources and references
src, status = oms.exportSnapshot("addExternalResources")
print(src, flush=True)

## add list of external resources from filesystem to ssp
oms.addResources("addExternalResources", "../../resources/externalRoot.ssv")
oms.addResources("addExternalResources:externalSystem.ssv", "../../resources/externalSystem1.ssv")
oms.addResources("addExternalResources", "../../resources/externalGain.ssv")

## delete the root references
oms.deleteResources("addExternalResources.root:root.ssv")
## switch with new references
oms.referenceResources("addExternalResources.root:externalRoot.ssv")

## delete the system references
oms.deleteResources("addExternalResources.root.system1:system1.ssv")
## switch with new references
oms.referenceResources("addExternalResources.root.system1:externalSystem.ssv")

## delete the component references
oms.deleteResources("addExternalResources.root.Gain:gain.ssv")
## switch with new references
oms.referenceResources("addExternalResources.root.Gain:externalGain.ssv")

oms.export("addExternalResources", "addExternalResources3.ssp")

oms.terminate("addExternalResources")
oms.delete("addExternalResources")

oms.importFile("addExternalResources3.ssp")

## new references and resources
src, status = oms.exportSnapshot("addExternalResources")
print(src)


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
##       name="addExternalResources"
##       version="1.0">
##       <ssd:System
##         name="root">
##         <ssd:Connectors>
##           <ssd:Connector
##             name="Input1"
##             kind="input">
##             <ssc:Real />
##           </ssd:Connector>
##           <ssd:Connector
##             name="Input2"
##             kind="input">
##             <ssc:Real />
##           </ssd:Connector>
##         </ssd:Connectors>
##         <ssd:ParameterBindings>
##           <ssd:ParameterBinding
##             source="resources/root.ssv" />
##         </ssd:ParameterBindings>
##         <ssd:Elements>
##           <ssd:System
##             name="system1">
##             <ssd:Connectors>
##               <ssd:Connector
##                 name="C1"
##                 kind="input">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="C2"
##                 kind="input">
##                 <ssc:Real />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:ParameterBindings>
##               <ssd:ParameterBinding
##                 source="resources/system1.ssv" />
##             </ssd:ParameterBindings>
##             <ssd:Annotations>
##               <ssc:Annotation
##                 type="org.openmodelica">
##                 <oms:Annotations>
##                   <oms:SimulationInformation>
##                     <oms:VariableStepSolver
##                       description="cvode"
##                       absoluteTolerance="0.0001"
##                       relativeTolerance="0.0001"
##                       minimumStepSize="1e-12"
##                       maximumStepSize="0.001"
##                       initialStepSize="1e-06" />
##                   </oms:SimulationInformation>
##                 </oms:Annotations>
##               </ssc:Annotation>
##             </ssd:Annotations>
##           </ssd:System>
##           <ssd:Component
##             name="Gain"
##             type="application/x-fmu-sharedlibrary"
##             source="resources/0001_Gain.fmu">
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
##                 <ssc:Real />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:ParameterBindings>
##               <ssd:ParameterBinding
##                 source="resources/gain.ssv" />
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
##       <ssd:DefaultExperiment
##         startTime="0.000000"
##         stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation
##                 resultFile="addExternalResources1.mat"
##                 loggingInterval="0.000000"
##                 bufferSize="10"
##                 signalFilter="resources/signalFilter.xml" />
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:DefaultExperiment>
##     </ssd:SystemStructureDescription>
##   </oms:file>
##   <oms:file
##     name="resources/root.ssv">
##     <ssv:ParameterSet
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       version="1.0"
##       name="parameters">
##       <ssv:Parameters>
##         <ssv:Parameter
##           name="Input2">
##           <ssv:Real
##             value="50" />
##         </ssv:Parameter>
##         <ssv:Parameter
##           name="Input1">
##           <ssv:Real
##             value="10" />
##         </ssv:Parameter>
##       </ssv:Parameters>
##     </ssv:ParameterSet>
##   </oms:file>
##   <oms:file
##     name="resources/system1.ssv">
##     <ssv:ParameterSet
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       version="1.0"
##       name="parameters">
##       <ssv:Parameters>
##         <ssv:Parameter
##           name="C1">
##           <ssv:Real
##             value="-10" />
##         </ssv:Parameter>
##       </ssv:Parameters>
##     </ssv:ParameterSet>
##   </oms:file>
##   <oms:file
##     name="resources/gain.ssv">
##     <ssv:ParameterSet
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       version="1.0"
##       name="parameters">
##       <ssv:Parameters>
##         <ssv:Parameter
##           name="k">
##           <ssv:Real
##             value="27"
##             unit="1" />
##         </ssv:Parameter>
##       </ssv:Parameters>
##       <ssv:Units>
##         <ssc:Unit
##           name="1">
##           <ssc:BaseUnit />
##         </ssc:Unit>
##       </ssv:Units>
##     </ssv:ParameterSet>
##   </oms:file>
##   <oms:file
##     name="resources/signalFilter.xml">
##     <oms:SignalFilter
##       version="1.0">
##       <oms:Variable
##         name="addExternalResources.root.Input1"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="addExternalResources.root.Input2"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="addExternalResources.root.Gain.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="addExternalResources.root.Gain.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="addExternalResources.root.Gain.k"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="addExternalResources.root.system1.C1"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="addExternalResources.root.system1.C2"
##         type="Real"
##         kind="input" />
##     </oms:SignalFilter>
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
##       name="addExternalResources"
##       version="1.0">
##       <ssd:System
##         name="root">
##         <ssd:Connectors>
##           <ssd:Connector
##             name="Input1"
##             kind="input">
##             <ssc:Real />
##           </ssd:Connector>
##           <ssd:Connector
##             name="Input2"
##             kind="input">
##             <ssc:Real />
##           </ssd:Connector>
##         </ssd:Connectors>
##         <ssd:ParameterBindings>
##           <ssd:ParameterBinding
##             source="resources/externalRoot.ssv" />
##         </ssd:ParameterBindings>
##         <ssd:Elements>
##           <ssd:System
##             name="system1">
##             <ssd:Connectors>
##               <ssd:Connector
##                 name="C1"
##                 kind="input">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="C2"
##                 kind="input">
##                 <ssc:Real />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:ParameterBindings>
##               <ssd:ParameterBinding
##                 source="resources/externalSystem.ssv" />
##             </ssd:ParameterBindings>
##             <ssd:Annotations>
##               <ssc:Annotation
##                 type="org.openmodelica">
##                 <oms:Annotations>
##                   <oms:SimulationInformation>
##                     <oms:VariableStepSolver
##                       description="cvode"
##                       absoluteTolerance="0.0001"
##                       relativeTolerance="0.0001"
##                       minimumStepSize="1e-12"
##                       maximumStepSize="0.001"
##                       initialStepSize="1e-06" />
##                   </oms:SimulationInformation>
##                 </oms:Annotations>
##               </ssc:Annotation>
##             </ssd:Annotations>
##           </ssd:System>
##           <ssd:Component
##             name="Gain"
##             type="application/x-fmu-sharedlibrary"
##             source="resources/0001_Gain.fmu">
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
##                 <ssc:Real />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:ParameterBindings>
##               <ssd:ParameterBinding
##                 source="resources/externalGain.ssv" />
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
##       <ssd:DefaultExperiment
##         startTime="0.000000"
##         stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation
##                 resultFile="addExternalResources1.mat"
##                 loggingInterval="0.000000"
##                 bufferSize="10"
##                 signalFilter="resources/signalFilter.xml" />
##             </oms:Annotations>
##           </ssc:Annotation>
##         </ssd:Annotations>
##       </ssd:DefaultExperiment>
##     </ssd:SystemStructureDescription>
##   </oms:file>
##   <oms:file
##     name="resources/externalRoot.ssv">
##     <ssv:ParameterSet
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       version="1.0"
##       name="parameters">
##       <ssv:Parameters>
##         <ssv:Parameter
##           name="Input2">
##           <ssv:Real
##             value="-500" />
##         </ssv:Parameter>
##         <ssv:Parameter
##           name="Input1">
##           <ssv:Real
##             value="-100" />
##         </ssv:Parameter>
##       </ssv:Parameters>
##     </ssv:ParameterSet>
##   </oms:file>
##   <oms:file
##     name="resources/externalSystem.ssv">
##     <ssv:ParameterSet
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       version="1.0"
##       name="parameters">
##       <ssv:Parameters>
##         <ssv:Parameter
##           name="C1">
##           <ssv:Real
##             value="-30" />
##         </ssv:Parameter>
##       </ssv:Parameters>
##     </ssv:ParameterSet>
##   </oms:file>
##   <oms:file
##     name="resources/externalGain.ssv">
##     <ssv:ParameterSet
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       version="1.0"
##       name="parameters">
##       <ssv:Parameters>
##         <ssv:Parameter
##           name="k">
##           <ssv:Real
##             value="-35"
##             unit="1" />
##         </ssv:Parameter>
##       </ssv:Parameters>
##       <ssv:Units>
##         <ssc:Unit
##           name="1">
##           <ssc:BaseUnit />
##         </ssc:Unit>
##       </ssv:Units>
##     </ssv:ParameterSet>
##   </oms:file>
##   <oms:file
##     name="resources/signalFilter.xml">
##     <oms:SignalFilter
##       version="1.0">
##       <oms:Variable
##         name="addExternalResources.root.Input1"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="addExternalResources.root.Input2"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="addExternalResources.root.Gain.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="addExternalResources.root.Gain.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="addExternalResources.root.Gain.k"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="addExternalResources.root.system1.C1"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="addExternalResources.root.system1.C2"
##         type="Real"
##         kind="input" />
##     </oms:SignalFilter>
##   </oms:file>
## </oms:snapshot>
##
## endResult
