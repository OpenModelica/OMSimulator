-- status: correct
-- teardown_command: rm -rf deleteStartValuesInSSV1_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: yes
-- mac: yes

oms_setCommandLineOption("--suppressPath=true")

oms_setTempDirectory("./deleteStartValuesInSSV1_lua/")
oms_setWorkingDirectory("./deleteStartValuesInSSV1_lua/")

oms_newModel("deleteStartValuesInSSV")

oms_addSystem("deleteStartValuesInSSV.Root", oms_system_wc)
oms_addConnector("deleteStartValuesInSSV.Root.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteStartValuesInSSV.Root.C2", oms_causality_output, oms_signal_type_real)

oms_addResources("deleteStartValuesInSSV.Root:root.ssv")

oms_setReal("deleteStartValuesInSSV.Root.C1", 10.0)
oms_setReal("deleteStartValuesInSSV.Root.C2", 15.0)

-- add subSystem
oms_addSystem("deleteStartValuesInSSV.Root.System1", oms_system_sc)
oms_addConnector("deleteStartValuesInSSV.Root.System1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteStartValuesInSSV.Root.System1.C2", oms_causality_input, oms_signal_type_real)
oms_addResources("deleteStartValuesInSSV.Root.System1:system1.ssv")

oms_setReal("deleteStartValuesInSSV.Root.System1.C1", 20.0)
oms_setReal("deleteStartValuesInSSV.Root.System1.C2", 30.0)

-- add submodel
oms_addSubModel("deleteStartValuesInSSV.Root.System1.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

oms_setReal("deleteStartValuesInSSV.Root.System1.Gain.k", 40.0)

oms_setResultFile("deleteStartValuesInSSV", "deleteStartValuesInSSV1.mat", 10)

src = oms_exportSnapshot("deleteStartValuesInSSV")
print(src)

-- delete top level system start value
oms_delete("deleteStartValuesInSSV.Root.C1:start")

-- delete subsystem start value
oms_delete("deleteStartValuesInSSV.Root.System1.C1:start")

-- delete fmu start value
oms_delete("deleteStartValuesInSSV.Root.System1.Gain.k:start")

src = oms_exportSnapshot("deleteStartValuesInSSV:resources/root.ssv")
print(src)

src = oms_exportSnapshot("deleteStartValuesInSSV:resources/system1.ssv")
print(src)

oms_delete("deleteStartValuesInSSV")


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
--       name="deleteStartValuesInSSV"
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
--             source="resources/root.ssv" />
--         </ssd:ParameterBindings>
--         <ssd:Elements>
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
--             <ssd:ParameterBindings>
--               <ssd:ParameterBinding
--                 source="resources/system1.ssv" />
--             </ssd:ParameterBindings>
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
--                 resultFile="deleteStartValuesInSSV1.mat"
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
--     name="resources/root.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="C2">
--           <ssv:Real
--             value="15" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="C1">
--           <ssv:Real
--             value="10" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/system1.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="Gain.k">
--           <ssv:Real
--             value="40" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="C2">
--           <ssv:Real
--             value="30" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="C1">
--           <ssv:Real
--             value="20" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="deleteStartValuesInSSV.Root.C1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteStartValuesInSSV.Root.C2"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="deleteStartValuesInSSV.Root.System1.C1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteStartValuesInSSV.Root.System1.C2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteStartValuesInSSV.Root.System1.Gain.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteStartValuesInSSV.Root.System1.Gain.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="deleteStartValuesInSSV.Root.System1.Gain.k"
--         type="Real"
--         kind="parameter" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- <?xml version="1.0"?>
-- <oms:snapshot
--   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--   partial="true">
--   <oms:file
--     name="resources/root.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="C2">
--           <ssv:Real
--             value="15" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
-- </oms:snapshot>
--
-- <?xml version="1.0"?>
-- <oms:snapshot
--   xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd"
--   partial="true">
--   <oms:file
--     name="resources/system1.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="C2">
--           <ssv:Real
--             value="30" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
