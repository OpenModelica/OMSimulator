## status: correct
## teardown_command: rm -rf importStartValues-py/
## linux: yes
## mac: no
## mingw32: yes
## mingw64: yes
## win: yes

import OMSimulator as oms

oms.setCommandLineOption('--suppressPath=true')
oms.setTempDirectory('./importStartValues-py/')

model = oms.importFile('../resources/importStartValues.ssp')

src1 = model.exportSnapshot()
print(src1)

model.delete()

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
##       name="importStartValues"
##       version="2.0">
##       <ssd:System
##         name="root">
##         <ssd:Connectors>
##           <ssd:Connector
##             name="C1"
##             kind="input">
##             <ssc:Real />
##           </ssd:Connector>
##         </ssd:Connectors>
##         <ssd:ParameterBindings>
##           <ssd:ParameterBinding
##             source="resources/Root.ssv" />
##           <ssd:ParameterBinding
##             source="resources/System1.ssv" />
##         </ssd:ParameterBindings>
##         <ssd:Elements>
##           <ssd:System
##             name="System1">
##             <ssd:Connectors>
##               <ssd:Connector
##                 name="C1"
##                 kind="input">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="C2"
##                 kind="parameter">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="C3"
##                 kind="output">
##                 <ssc:Real />
##               </ssd:Connector>
##             </ssd:Connectors>
##             <ssd:Annotations>
##               <ssc:Annotation
##                 type="org.openmodelica">
##                 <oms:Annotations>
##                   <oms:SimulationInformation>
##                     <oms:VariableStepSolver
##                       description="euler"
##                       relativeTolerance="0.0001"
##                       minimumStepSize="0.0001"
##                       maximumStepSize="0.1"
##                       initialStepSize="0.0001" />
##                   </oms:SimulationInformation>
##                 </oms:Annotations>
##               </ssc:Annotation>
##             </ssd:Annotations>
##           </ssd:System>
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
##       <ssd:DefaultExperiment
##         startTime="0.000000"
##         stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation
##                 resultFile="importStartValues_res.mat"
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
##     name="resources/Root.ssv">
##     <ssv:ParameterSet
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       version="2.0"
##       name="parameters">
##       <ssv:Parameters>
##         <ssv:Parameter
##           name="C1">
##           <ssv:Real
##             value="-10.5" />
##         </ssv:Parameter>
##       </ssv:Parameters>
##     </ssv:ParameterSet>
##   </oms:file>
##   <oms:file
##     name="resources/System1.ssv">
##     <ssv:ParameterSet
##       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
##       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
##       version="2.0"
##       name="parameters">
##       <ssv:Parameters>
##         <ssv:Parameter
##           name="System1.C2">
##           <ssv:Real
##             value="3.5" />
##         </ssv:Parameter>
##         <ssv:Parameter
##           name="System1.C1">
##           <ssv:Real
##             value="-13.5" />
##         </ssv:Parameter>
##       </ssv:Parameters>
##     </ssv:ParameterSet>
##   </oms:file>
##   <oms:file
##     name="resources/signalFilter.xml">
##     <oms:SignalFilter
##       version="2.0">
##       <oms:Variable
##         name="importStartValues.root.C1"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="importStartValues.root.System1.C1"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="importStartValues.root.System1.C2"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="importStartValues.root.System1.C3"
##         type="Real"
##         kind="output" />
##     </oms:SignalFilter>
##   </oms:file>
## </oms:snapshot>
##
## endResult
