## status: correct
## teardown_command: rm -rf replacesubmodel_11_py/
## linux: no
## mingw32: no
## mingw64: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./replacesubmodel_11_py/")
oms.setWorkingDirectory("./replacesubmodel_11_py/")

oms.newModel("model")

oms.addSystem("model.root", oms.system_wc)

oms.addSubModel("model.root.A", "../../resources/Modelica.Blocks.Sources.Sine.fmu")
oms.addSubModel("model.root.B", "../../resources/Modelica.Blocks.Math.Gain.fmu")

## add connections
oms.addConnection("model.root.A.y", "model.root.B.u")

oms.setResultFile("model", "replaceSubmodel11.mat")

src, status = oms.exportSnapshot("model")
print(src)

oms.export("model", "replaceSubmodel11.ssp")
oms.terminate("model")
oms.delete("model")

oms.importFile("replaceSubmodel11.ssp")

oms.replaceSubModel("model.root.B", "../../resources/Modelica.Blocks.Math.Gain.fmu", False)

src, status = oms.exportSnapshot("model")
print(src)


oms.terminate("model")
oms.delete("model")

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
##       version="1.0">
##       <ssd:System
##         name="root">
##         <ssd:Elements>
##           <ssd:Component
##             name="B"
##             type="application/x-fmu-sharedlibrary"
##             source="resources/0002_B.fmu">
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
##           </ssd:Component>
##           <ssd:Component
##             name="A"
##             type="application/x-fmu-sharedlibrary"
##             source="resources/0001_A.fmu">
##             <ssd:Connectors>
##               <ssd:Connector
##                 name="y"
##                 kind="output">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="1.000000"
##                   y="0.500000" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="amplitude"
##                 kind="parameter">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="freqHz"
##                 kind="parameter">
##                 <ssc:Real
##                   unit="Hz" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="offset"
##                 kind="parameter">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="phase"
##                 kind="parameter">
##                 <ssc:Real
##                   unit="rad" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="startTime"
##                 kind="parameter">
##                 <ssc:Real
##                   unit="s" />
##               </ssd:Connector>
##             </ssd:Connectors>
##           </ssd:Component>
##         </ssd:Elements>
##         <ssd:Connections>
##           <ssd:Connection
##             startElement="A"
##             startConnector="y"
##             endElement="B"
##             endConnector="u" />
##         </ssd:Connections>
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
##         <ssc:Unit
##           name="Hz">
##           <ssc:BaseUnit
##             s="-1" />
##         </ssc:Unit>
##         <ssc:Unit
##           name="rad">
##           <ssc:BaseUnit />
##         </ssc:Unit>
##         <ssc:Unit
##           name="s">
##           <ssc:BaseUnit
##             s="1" />
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
##                 resultFile="replaceSubmodel11.mat"
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
##         name="model.root.B.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="model.root.B.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.B.k"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.A.amplitude"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.freqHz"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.offset"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.phase"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.startTime"
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
##             name="B"
##             type="application/x-fmu-sharedlibrary"
##             source="resources/0002_B.fmu">
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
##           </ssd:Component>
##           <ssd:Component
##             name="A"
##             type="application/x-fmu-sharedlibrary"
##             source="resources/0001_A.fmu">
##             <ssd:Connectors>
##               <ssd:Connector
##                 name="y"
##                 kind="output">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="1.000000"
##                   y="0.500000" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="amplitude"
##                 kind="parameter">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="freqHz"
##                 kind="parameter">
##                 <ssc:Real
##                   unit="Hz" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="offset"
##                 kind="parameter">
##                 <ssc:Real />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="phase"
##                 kind="parameter">
##                 <ssc:Real
##                   unit="rad" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="startTime"
##                 kind="parameter">
##                 <ssc:Real
##                   unit="s" />
##               </ssd:Connector>
##             </ssd:Connectors>
##           </ssd:Component>
##         </ssd:Elements>
##         <ssd:Connections>
##           <ssd:Connection
##             startElement="A"
##             startConnector="y"
##             endElement="B"
##             endConnector="u" />
##         </ssd:Connections>
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
##         <ssc:Unit
##           name="Hz">
##           <ssc:BaseUnit
##             s="-1" />
##         </ssc:Unit>
##         <ssc:Unit
##           name="rad">
##           <ssc:BaseUnit />
##         </ssc:Unit>
##         <ssc:Unit
##           name="s">
##           <ssc:BaseUnit
##             s="1" />
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
##                 resultFile="replaceSubmodel11.mat"
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
##         name="model.root.B.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="model.root.B.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.B.k"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.A.amplitude"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.freqHz"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.offset"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.phase"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.startTime"
##         type="Real"
##         kind="parameter" />
##     </oms:SignalFilter>
##   </oms:file>
## </oms:snapshot>
##
## endResult
