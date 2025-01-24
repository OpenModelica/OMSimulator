## status: correct
## teardown_command: rm -rf replacesubmodel_06_py/
## linux: no
## mingw32: no
## mingw64: yes
## win: no
## mac: no

from OMSimulator import OMSimulator
oms = OMSimulator()

oms.setCommandLineOption("--suppressPath=true")
oms.setTempDirectory("./replacesubmodel_06_py/")
oms.setWorkingDirectory("./replacesubmodel_06_py/")

oms.newModel("model")

oms.addSystem("model.root", oms.system_wc)

oms.addSubModel("model.root.A", "../../resources/replaceA.fmu")
oms.addSubModel("model.root.B", "../../resources/replaceB.fmu")

oms.setReal("model.root.A.u", 10.0)
oms.setReal("model.root.A.t", -10.0)
oms.setReal("model.root.B.u1", -13.0)
oms.setReal("model.root.B.z", -15.0)

## add connections
oms.addConnection("model.root.A.y", "model.root.B.u")
oms.addConnection("model.root.A.dummy", "model.root.B.u1")

oms.setResultFile("model", "replaceSubmodel6.mat")

src, status = oms.exportSnapshot("model")
print(src)

print("info:    Before replacing the Model")
print("info:      model.root.A.u      : " , oms.getReal("model.root.A.u")[0])
print("info:      model.root.B.u      : " , oms.getReal("model.root.B.u")[0])
print("info:      model.root.B.u1     : " , oms.getReal("model.root.B.u1")[0])
print("info:      model.root.B.z      : " , oms.getReal("model.root.B.z")[0])

oms.export("model", "replaceSubmodel6.ssp")
oms.terminate("model")
oms.delete("model")

oms.importFile("replaceSubmodel6.ssp")

oms.replaceSubModel("model.root.A", "../../resources/replaceA_extended.fmu", False)

src, status = oms.exportSnapshot("model")
print(src)

print("info:    After replacing the Model")
print("info:      model.root.A.u      : " , oms.getReal("model.root.A.u")[0])
print("info:      model.root.B.u      : " , oms.getReal("model.root.B.u")[0])
print("info:      model.root.B.u1     : " , oms.getReal("model.root.B.u1")[0])
print("info:      model.root.B.z      : " , oms.getReal("model.root.B.z")[0])


oms.instantiate("model")
oms.initialize("model")

print("info:    Initialize")
print("info:      model.root.A.u      : " , oms.getReal("model.root.A.u")[0])
print("info:      model.root.A.y      : " , oms.getReal("model.root.A.y")[0])
print("info:      model.root.B.u      : " , oms.getReal("model.root.B.u")[0])
print("info:      model.root.B.u1     : " , oms.getReal("model.root.B.u1")[0])
print("info:      model.root.B.z      : " , oms.getReal("model.root.B.z")[0])

oms.simulate("model")

print("info:    Simulate")
print("info:      model.root.A.u      : " , oms.getReal("model.root.A.u")[0])
print("info:      model.root.A.y      : " , oms.getReal("model.root.A.y")[0])
print("info:      model.root.B.u      : " , oms.getReal("model.root.B.u")[0])
print("info:      model.root.B.u1     : " , oms.getReal("model.root.B.u1")[0])
print("info:      model.root.B.z      : " , oms.getReal("model.root.B.z")[0])

oms.terminate("model")
oms.delete("model")

## Result:
## error:   [getVariable] Unknown signal "model.root.A.dummy"
## warning: deleting connection "A.dummy ==> B.u1", as signal "dummy" couldn't be resolved to any signal in the replaced submodel "../../resources/replaceA_extended.fmu"
## warning: deleting start value "A.t" in "inline" resources, because the identifier couldn't be resolved to any system signal in the replacing model
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
##                   y="0.333333" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="u1"
##                 kind="input">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="0.000000"
##                   y="0.666667" />
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
##                 name="z"
##                 kind="parameter">
##                 <ssc:Real />
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
##                         name="z">
##                         <ssv:Real
##                           value="-15" />
##                       </ssv:Parameter>
##                       <ssv:Parameter
##                         name="u1">
##                         <ssv:Real
##                           value="-13" />
##                       </ssv:Parameter>
##                     </ssv:Parameters>
##                   </ssv:ParameterSet>
##                 </ssd:ParameterValues>
##               </ssd:ParameterBinding>
##             </ssd:ParameterBindings>
##           </ssd:Component>
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
##                 name="dummy"
##                 kind="output">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="1.000000"
##                   y="0.333333" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="y"
##                 kind="output">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="1.000000"
##                   y="0.666667" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="t"
##                 kind="parameter">
##                 <ssc:Real />
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
##                           value="10" />
##                       </ssv:Parameter>
##                       <ssv:Parameter
##                         name="t">
##                         <ssv:Real
##                           value="-10" />
##                       </ssv:Parameter>
##                     </ssv:Parameters>
##                   </ssv:ParameterSet>
##                 </ssd:ParameterValues>
##               </ssd:ParameterBinding>
##             </ssd:ParameterBindings>
##           </ssd:Component>
##         </ssd:Elements>
##         <ssd:Connections>
##           <ssd:Connection
##             startElement="A"
##             startConnector="y"
##             endElement="B"
##             endConnector="u" />
##           <ssd:Connection
##             startElement="A"
##             startConnector="dummy"
##             endElement="B"
##             endConnector="u1" />
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
##       <ssd:DefaultExperiment
##         startTime="0.000000"
##         stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation
##                 resultFile="replaceSubmodel6.mat"
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
##         name="model.root.B.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="model.root.B.u1"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="model.root.B.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.B.z"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.x"
##         type="Real"
##         kind="unknown" />
##       <oms:Variable
##         name="model.root.A.der(x)"
##         type="Real"
##         kind="unknown" />
##       <oms:Variable
##         name="model.root.A.dummy"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.A.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="model.root.A.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.A.t"
##         type="Real"
##         kind="parameter" />
##     </oms:SignalFilter>
##   </oms:file>
## </oms:snapshot>
##
## info:    Before replacing the Model
## info:      model.root.A.u      :  10.0
## info:      model.root.B.u      :  1.0
## info:      model.root.B.u1     :  -13.0
## info:      model.root.B.z      :  -15.0
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
##                   y="0.333333" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="u1"
##                 kind="input">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="0.000000"
##                   y="0.666667" />
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
##                 name="z"
##                 kind="parameter">
##                 <ssc:Real />
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
##                         name="z">
##                         <ssv:Real
##                           value="-15" />
##                       </ssv:Parameter>
##                       <ssv:Parameter
##                         name="u1">
##                         <ssv:Real
##                           value="-13" />
##                       </ssv:Parameter>
##                     </ssv:Parameters>
##                   </ssv:ParameterSet>
##                 </ssd:ParameterValues>
##               </ssd:ParameterBinding>
##             </ssd:ParameterBindings>
##           </ssd:Component>
##           <ssd:Component
##             name="A"
##             type="application/x-fmu-sharedlibrary"
##             source="resources/0003_replaceA.fmu">
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
##                   y="0.333333" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="y1"
##                 kind="output">
##                 <ssc:Real />
##                 <ssd:ConnectorGeometry
##                   x="1.000000"
##                   y="0.666667" />
##               </ssd:Connector>
##               <ssd:Connector
##                 name="foo"
##                 kind="parameter">
##                 <ssc:Real />
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
##                           value="10" />
##                       </ssv:Parameter>
##                     </ssv:Parameters>
##                   </ssv:ParameterSet>
##                 </ssd:ParameterValues>
##               </ssd:ParameterBinding>
##             </ssd:ParameterBindings>
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
##       <ssd:DefaultExperiment
##         startTime="0.000000"
##         stopTime="1.000000">
##         <ssd:Annotations>
##           <ssc:Annotation
##             type="org.openmodelica">
##             <oms:Annotations>
##               <oms:SimulationInformation
##                 resultFile="replaceSubmodel6.mat"
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
##         name="model.root.B.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="model.root.B.u1"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="model.root.B.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.B.z"
##         type="Real"
##         kind="parameter" />
##       <oms:Variable
##         name="model.root.A.x"
##         type="Real"
##         kind="unknown" />
##       <oms:Variable
##         name="model.root.A.der(x)"
##         type="Real"
##         kind="unknown" />
##       <oms:Variable
##         name="model.root.A.u"
##         type="Real"
##         kind="input" />
##       <oms:Variable
##         name="model.root.A.y"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.A.y1"
##         type="Real"
##         kind="output" />
##       <oms:Variable
##         name="model.root.A.foo"
##         type="Real"
##         kind="parameter" />
##     </oms:SignalFilter>
##   </oms:file>
## </oms:snapshot>
## info:    Result file: replaceSubmodel6.mat (bufferSize=1)
##
## info:    After replacing the Model
## info:      model.root.A.u      :  10.0
## info:      model.root.B.u      :  1.0
## info:      model.root.B.u1     :  -13.0
## info:      model.root.B.z      :  -15.0
## info:    Initialize
## info:      model.root.A.u      :  10.0
## info:      model.root.A.y      :  10.0
## info:      model.root.B.u      :  10.0
## info:      model.root.B.u1     :  -13.0
## info:      model.root.B.z      :  -15.0
## info:    Simulate
## info:      model.root.A.u      :  10.0
## info:      model.root.A.y      :  10.0
## info:      model.root.B.u      :  10.0
## info:      model.root.B.u1     :  -13.0
## info:      model.root.B.z      :  -15.0
## info:    2 warnings
## info:    1 errors
## endResult
