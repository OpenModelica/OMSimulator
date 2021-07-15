-- status: correct
-- teardown_command: rm -rf deleteResources1_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./deleteResources1_lua/")
oms_setWorkingDirectory("./deleteResources1_lua/")

oms_newModel("deleteResources")

oms_addSystem("deleteResources.root", oms_system_wc)
oms_addConnector("deleteResources.root.Input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteResources.root.Input2", oms_causality_input, oms_signal_type_real)

-- add Top level resources
oms_addResources("deleteResources.root:root.ssv")

oms_setReal("deleteResources.root.Input1", 10)
oms_setReal("deleteResources.root.Input2", 50)

oms_addSystem("deleteResources.root.system1", oms_system_sc)
oms_addConnector("deleteResources.root.system1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteResources.root.system1.C2", oms_causality_input, oms_signal_type_real)

-- add resources to subsystem
oms_addResources("deleteResources.root.system1:system1.ssv")
oms_setReal("deleteResources.root.system1.C1", -10)

oms_addSubModel("deleteResources.root.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

-- add resources to submodule
oms_addResources("deleteResources.root.Gain:gain.ssv")

oms_setReal("deleteResources.root.Gain.k", 27)

oms_setResultFile("deleteResources", "deleteResources1.mat", 10)

oms_export("deleteResources", "deleteResources1.ssp")

oms_terminate("deleteResources")
oms_delete("deleteResources")

oms_importFile("deleteResources1.ssp")

-- print original snapshot
src = oms_exportSnapshot("deleteResources")
print(src)

-- delete only the references
oms_deleteResources("deleteResources.root:root.ssv")
-- delete only the references
oms_deleteResources("deleteResources.root.system1:system1.ssv")
-- delete only the references
oms_deleteResources("deleteResources.root.Gain:gain.ssv")

-- TODO delete from resources and all refereences
-- oms_deleteResources("deleteResources:gain.ssv")

-- adding new resources to exisiting API, -- skip it as of now
-- oms_deleteResources2("deleteResources", "newResources.ssv")

-- snapshot after deleting references
src = oms_exportSnapshot("deleteResources")
print(src)

oms_terminate("deleteResources")
oms_delete("deleteResources")

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
--       name="deleteResources"
--       version="1.0">
--       <ssd:System
--         name="root">
--         <ssd:Connectors>
--           <ssd:Connector
--             name="Input1"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="Input2"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--         </ssd:Connectors>
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding
--             source="resources/root.ssv" />
--         </ssd:ParameterBindings>
--         <ssd:Elements>
--           <ssd:System
--             name="system1">
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
--           <ssd:Component
--             name="Gain"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0001_Gain.fmu">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="u"
--                 kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="y"
--                 kind="output">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="1.000000"
--                   y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="k"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:ParameterBindings>
--               <ssd:ParameterBinding
--                 source="resources/gain.ssv" />
--             </ssd:ParameterBindings>
--           </ssd:Component>
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
--                 resultFile="deleteResources1.mat"
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
--           name="Input2">
--           <ssv:Real
--             value="50" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="Input1">
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
--           name="C1">
--           <ssv:Real
--             value="-10" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/gain.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="k">
--           <ssv:Real
--             value="27" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="deleteResources.root.Input1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteResources.root.Input2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteResources.root.Gain.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteResources.root.Gain.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="deleteResources.root.Gain.k"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="deleteResources.root.system1.C1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteResources.root.system1.C2"
--         type="Real"
--         kind="input" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
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
--       name="deleteResources"
--       version="1.0">
--       <ssd:System
--         name="root">
--         <ssd:Connectors>
--           <ssd:Connector
--             name="Input1"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="Input2"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--         </ssd:Connectors>
--         <ssd:Elements>
--           <ssd:System
--             name="system1">
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
--           <ssd:Component
--             name="Gain"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0001_Gain.fmu">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="u"
--                 kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="y"
--                 kind="output">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="1.000000"
--                   y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="k"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--           </ssd:Component>
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
--                 resultFile="deleteResources1.mat"
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
--           name="Input2">
--           <ssv:Real
--             value="50" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="Input1">
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
--           name="C1">
--           <ssv:Real
--             value="-10" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/gain.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="k">
--           <ssv:Real
--             value="27" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="deleteResources.root.Input1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteResources.root.Input2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteResources.root.Gain.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteResources.root.Gain.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="deleteResources.root.Gain.k"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="deleteResources.root.system1.C1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="deleteResources.root.system1.C2"
--         type="Real"
--         kind="input" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
