-- status: correct
-- teardown_command: rm -rf deleteConnector1_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: no
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./deleteConnector1_lua/")
oms_setWorkingDirectory("./deleteConnector1_lua/")

oms_newModel("deleteConnector")

oms_addSystem("deleteConnector.Root", oms_system_wc)

oms_addResources("deleteConnector.Root:deleteConnector.ssv")

oms_addConnector("deleteConnector.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.C2", oms_causality_output, oms_signal_type_real)
oms_setReal("deleteConnector.Root.C1", -70.0)

oms_addConnection("deleteConnector.Root.C1", "deleteConnector.Root.C2")

oms_addSystem("deleteConnector.Root.System1", oms_system_sc)
oms_addConnector("deleteConnector.Root.System1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.System1.C2", oms_causality_input, oms_signal_type_real)

oms_addSystem("deleteConnector.Root.System2", oms_system_sc)
oms_addConnector("deleteConnector.Root.System2.C3", oms_causality_output, oms_signal_type_real)
oms_addConnector("deleteConnector.Root.System2.C4", oms_causality_output, oms_signal_type_real)
oms_setReal("deleteConnector.Root.System2.C3", 20.0)
oms_setReal("deleteConnector.Root.System2.C4", 30.0)

oms_addSubModel("deleteConnector.Root.System1.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")
oms_setReal("deleteConnector.Root.System1.Gain.k", 30.0)

oms_addConnection("deleteConnector.Root.System2.C3", "deleteConnector.Root.System1.C1")
oms_addConnection("deleteConnector.Root.System2.C4", "deleteConnector.Root.System1.C2")

oms_setResultFile("deleteConnector", "deleteConnector1.mat", 10)

src = oms_exportSnapshot("deleteConnector")
print(src)

oms_delete("deleteConnector.Root.C1")

-- delete connector "deleteConnector.Root.System2.C3" and associated connection
oms_delete("deleteConnector.Root.System2.C3")

-- delete components connector and its startValue
oms_delete("deleteConnector.Root.System1.Gain.k")

-- delete unknown connector signal for warning
oms_delete("deleteConnector.Root.System2.C6")

-- delete unknown system for warning
oms_delete("deleteConnector.Root.System3")

-- after deletion
src = oms_exportSnapshot("deleteConnector:resources/deleteConnector.ssv")
print(src)

oms_terminate("deleteConnector")
oms_delete("deleteConnector")


-- Result:
-- <?xml version="1.0"?>
-- <oms:snapshot
--   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--   partial="false">
--   <oms:file
--     name="SystemStructure.ssd">
--     <ssd:SystemStructureDescription
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping"
--       xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary"
--       xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--       name="deleteConnector"
--       version="1.0">
--       <ssd:System
--         name="Root">
--         <ssd:Connectors>
--           <ssd:Connector
--             name="C1"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="C2"
--             kind="output">
--             <ssc:Real />
--           </ssd:Connector>
--         </ssd:Connectors>
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding
--             source="resources/deleteConnector.ssv" />
--         </ssd:ParameterBindings>
--         <ssd:Elements>
--           <ssd:System
--             name="System2">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="C3"
--                 kind="output">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="C4"
--                 kind="output">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:Annotations>
--               <ssc:Annotation
--                 type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:SimulationInformation>
--                     <oms:VariableStepSolver
--                       description="cvode"
--                       absoluteTolerance="0.0001"
--                       relativeTolerance="0.0001"
--                       minimumStepSize="1e-12"
--                       maximumStepSize="0.001"
--                       initialStepSize="1e-06" />
--                   </oms:SimulationInformation>
--                 </oms:Annotations>
--               </ssc:Annotation>
--             </ssd:Annotations>
--           </ssd:System>
--           <ssd:System
--             name="System1">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="C1"
--                 kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="C2"
--                 kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:Elements>
--               <ssd:Component
--                 name="Gain"
--                 type="application/x-fmu-sharedlibrary"
--                 source="resources/0001_Gain.fmu">
--                 <ssd:Connectors>
--                   <ssd:Connector
--                     name="u"
--                     kind="input">
--                     <ssc:Real />
--                     <ssd:ConnectorGeometry
--                       x="0.000000"
--                       y="0.500000" />
--                   </ssd:Connector>
--                   <ssd:Connector
--                     name="y"
--                     kind="output">
--                     <ssc:Real />
--                     <ssd:ConnectorGeometry
--                       x="1.000000"
--                       y="0.500000" />
--                   </ssd:Connector>
--                   <ssd:Connector
--                     name="k"
--                     kind="parameter">
--                     <ssc:Real />
--                   </ssd:Connector>
--                 </ssd:Connectors>
--               </ssd:Component>
--             </ssd:Elements>
--             <ssd:Annotations>
--               <ssc:Annotation
--                 type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:SimulationInformation>
--                     <oms:VariableStepSolver
--                       description="cvode"
--                       absoluteTolerance="0.0001"
--                       relativeTolerance="0.0001"
--                       minimumStepSize="1e-12"
--                       maximumStepSize="0.001"
--                       initialStepSize="1e-06" />
--                   </oms:SimulationInformation>
--                 </oms:Annotations>
--               </ssc:Annotation>
--             </ssd:Annotations>
--           </ssd:System>
--         </ssd:Elements>
--         <ssd:Connections>
--           <ssd:Connection
--             startElement=""
--             startConnector="C1"
--             endElement=""
--             endConnector="C2" />
--           <ssd:Connection
--             startElement="System2"
--             startConnector="C3"
--             endElement="System1"
--             endConnector="C1" />
--           <ssd:Connection
--             startElement="System2"
--             startConnector="C4"
--             endElement="System1"
--             endConnector="C2" />
--         </ssd:Connections>
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation>
--                 <oms:FixedStepMaster
--                   description="oms-ma"
--                   stepSize="0.001000"
--                   absoluteTolerance="0.000100"
--                   relativeTolerance="0.000100" />
--               </oms:SimulationInformation>
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:System>
--       <ssd:DefaultExperiment
--         startTime="0.000000"
--         stopTime="1.000000">
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation
--                 resultFile="deleteConnector1.mat"
--                 loggingInterval="0.000000"
--                 bufferSize="10"
--                 signalFilter="resources/signalFilter.xml" />
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:DefaultExperiment>
--     </ssd:SystemStructureDescription>
--   </oms:file>
--   <oms:file
--     name="resources/deleteConnector.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="System2.C4">
--           <ssv:Real
--             value="30" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="System2.C3">
--           <ssv:Real
--             value="20" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="Gain.k">
--           <ssv:Real
--             value="30" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="C1">
--           <ssv:Real
--             value="-70" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="deleteConnector.Root.C1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteConnector.Root.C2"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="deleteConnector.Root.System2.C3"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="deleteConnector.Root.System2.C4"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="deleteConnector.Root.System1.C1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteConnector.Root.System1.C2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteConnector.Root.System1.Gain.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteConnector.Root.System1.Gain.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="deleteConnector.Root.System1.Gain.k"
--         type="Real"
--         kind="parameter" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- warning: failed to delete object "deleteConnector.Root.System2.C6" because the identifier couldn't be resolved to any connector, component, system, or model
-- warning: failed to delete object "deleteConnector.Root.System3" because the identifier couldn't be resolved to any connector, component, system, or model
-- <?xml version="1.0"?>
-- <oms:snapshot
--   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--   partial="true">
--   <oms:file
--     name="resources/deleteConnector.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="System2.C4">
--           <ssv:Real
--             value="30" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    2 warnings
-- info:    0 errors
-- endResult
