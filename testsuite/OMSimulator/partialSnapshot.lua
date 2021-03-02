-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
oms_setTempDirectory("./partial_snapshot_lua/")

oms_newModel("snapshot")
oms_addSystem("snapshot.root", oms_system_wc)

oms_addConnector("snapshot.root.C1", oms_causality_input, oms_signal_type_real)
oms_setReal("snapshot.root.C1", -10)

oms_addSubModel("snapshot.root.add", "../resources/Modelica.Blocks.Math.Add.fmu")
oms_setReal("snapshot.root.add.u1", 10)
oms_setReal("snapshot.root.add.k1", 30)

-- correct, querying full model
snapshot = oms_exportSnapshot("snapshot")
print(snapshot)

-- correct, querying partial snapshot ".ssd"
snapshot = oms_exportSnapshot("snapshot:SystemStructure.ssd")
print(snapshot)

-- correct, querying partial snapshot ".ssv"
snapshot = oms_exportSnapshot("snapshot:resources/snapshot.ssv")
print(snapshot)

-- error querying wrong resources
oms_exportSnapshot("snapshot:resources/snapshot1.ssv")

-- error querying wrong model
oms_exportSnapshot("snapshot1:resources/snapshot1.ssv")

-- query system
snapshot = oms_exportSnapshot("snapshot.root:SystemStructure.ssd")
print(snapshot)

-- query sub component
snapshot = oms_exportSnapshot("snapshot.root.add:SystemStructure.ssd")
print(snapshot)

-- TODO error messages for querying wrong subsystems or components
-- snapshot = oms_exportSnapshot("snapshot.root.add1:SystemStructure.ssd")


-- Result:
-- <?xml version="1.0"?>
-- <oms:snapshot partial="false">
--   <oms:file name="SystemStructure.ssd">
--     <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="snapshot" version="1.0">
--       <ssd:System name="root">
--         <ssd:Connectors>
--           <ssd:Connector name="C1" kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--         </ssd:Connectors>
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding source="resources/snapshot.ssv" />
--         </ssd:ParameterBindings>
--         <ssd:Elements>
--           <ssd:Component name="add" type="application/x-fmu-sharedlibrary" source="resources/0001_add.fmu">
--             <ssd:Connectors>
--               <ssd:Connector name="u1" kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry x="0.000000" y="0.333333" />
--               </ssd:Connector>
--               <ssd:Connector name="u2" kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry x="0.000000" y="0.666667" />
--               </ssd:Connector>
--               <ssd:Connector name="y" kind="output">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry x="1.000000" y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector name="k1" kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector name="k2" kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--           </ssd:Component>
--         </ssd:Elements>
--         <ssd:Annotations>
--           <ssc:Annotation type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation>
--                 <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
--               </oms:SimulationInformation>
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:System>
--       <ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
--         <ssd:Annotations>
--           <ssc:Annotation type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation resultFile="snapshot_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:DefaultExperiment>
--     </ssd:SystemStructureDescription>
--   </oms:file>
--   <oms:file name="resources/snapshot.ssv">
--     <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="1.0" name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter name="C1">
--           <ssv:Real value="-10" />
--         </ssv:Parameter>
--         <ssv:Parameter name="add.u1">
--           <ssv:Real value="10" />
--         </ssv:Parameter>
--         <ssv:Parameter name="add.k1">
--           <ssv:Real value="30" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
-- </oms:snapshot>
--
-- <?xml version="1.0"?>
-- <oms:snapshot partial="true">
--   <oms:file name="SystemStructure.ssd">
--     <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="snapshot" version="1.0">
--       <ssd:System name="root">
--         <ssd:Connectors>
--           <ssd:Connector name="C1" kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--         </ssd:Connectors>
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding source="resources/snapshot.ssv" />
--         </ssd:ParameterBindings>
--         <ssd:Elements>
--           <ssd:Component name="add" type="application/x-fmu-sharedlibrary" source="resources/0001_add.fmu">
--             <ssd:Connectors>
--               <ssd:Connector name="u1" kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry x="0.000000" y="0.333333" />
--               </ssd:Connector>
--               <ssd:Connector name="u2" kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry x="0.000000" y="0.666667" />
--               </ssd:Connector>
--               <ssd:Connector name="y" kind="output">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry x="1.000000" y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector name="k1" kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector name="k2" kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--           </ssd:Component>
--         </ssd:Elements>
--         <ssd:Annotations>
--           <ssc:Annotation type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation>
--                 <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
--               </oms:SimulationInformation>
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:System>
--       <ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
--         <ssd:Annotations>
--           <ssc:Annotation type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation resultFile="snapshot_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:DefaultExperiment>
--     </ssd:SystemStructureDescription>
--   </oms:file>
-- </oms:snapshot>
--
-- <?xml version="1.0"?>
-- <oms:snapshot partial="true">
--   <oms:file name="resources/snapshot.ssv">
--     <ssv:ParameterSet xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" version="1.0" name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter name="C1">
--           <ssv:Real value="-10" />
--         </ssv:Parameter>
--         <ssv:Parameter name="add.u1">
--           <ssv:Real value="10" />
--         </ssv:Parameter>
--         <ssv:Parameter name="add.k1">
--           <ssv:Real value="30" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
-- </oms:snapshot>
--
-- error:   [getResourceNode] Failed to find node "resources/snapshot1.ssv"
-- error:   [exportPartialSnapshot] Failed to find node "resources/snapshot1.ssv"
-- error:   [oms_exportSnapshot] Model "snapshot1:resources/snapshot1.ssv" does not exist in the scope
-- <?xml version="1.0"?>
-- <oms:snapshot partial="true">
--   <oms:file name="SystemStructure.ssd" node="snapshot.root">
--     <ssd:System name="root">
--       <ssd:Connectors>
--         <ssd:Connector name="C1" kind="input">
--           <ssc:Real />
--         </ssd:Connector>
--       </ssd:Connectors>
--       <ssd:ParameterBindings>
--         <ssd:ParameterBinding source="resources/snapshot.ssv" />
--       </ssd:ParameterBindings>
--       <ssd:Elements>
--         <ssd:Component name="add" type="application/x-fmu-sharedlibrary" source="resources/0001_add.fmu">
--           <ssd:Connectors>
--             <ssd:Connector name="u1" kind="input">
--               <ssc:Real />
--               <ssd:ConnectorGeometry x="0.000000" y="0.333333" />
--             </ssd:Connector>
--             <ssd:Connector name="u2" kind="input">
--               <ssc:Real />
--               <ssd:ConnectorGeometry x="0.000000" y="0.666667" />
--             </ssd:Connector>
--             <ssd:Connector name="y" kind="output">
--               <ssc:Real />
--               <ssd:ConnectorGeometry x="1.000000" y="0.500000" />
--             </ssd:Connector>
--             <ssd:Connector name="k1" kind="parameter">
--               <ssc:Real />
--             </ssd:Connector>
--             <ssd:Connector name="k2" kind="parameter">
--               <ssc:Real />
--             </ssd:Connector>
--           </ssd:Connectors>
--         </ssd:Component>
--       </ssd:Elements>
--       <ssd:Annotations>
--         <ssc:Annotation type="org.openmodelica">
--           <oms:Annotations>
--             <oms:SimulationInformation>
--               <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
--             </oms:SimulationInformation>
--           </oms:Annotations>
--         </ssc:Annotation>
--       </ssd:Annotations>
--     </ssd:System>
--   </oms:file>
-- </oms:snapshot>
--
-- <?xml version="1.0"?>
-- <oms:snapshot partial="true">
--   <oms:file name="SystemStructure.ssd" node="snapshot.root.add">
--     <ssd:Component name="add" type="application/x-fmu-sharedlibrary" source="resources/0001_add.fmu">
--       <ssd:Connectors>
--         <ssd:Connector name="u1" kind="input">
--           <ssc:Real />
--           <ssd:ConnectorGeometry x="0.000000" y="0.333333" />
--         </ssd:Connector>
--         <ssd:Connector name="u2" kind="input">
--           <ssc:Real />
--           <ssd:ConnectorGeometry x="0.000000" y="0.666667" />
--         </ssd:Connector>
--         <ssd:Connector name="y" kind="output">
--           <ssc:Real />
--           <ssd:ConnectorGeometry x="1.000000" y="0.500000" />
--         </ssd:Connector>
--         <ssd:Connector name="k1" kind="parameter">
--           <ssc:Real />
--         </ssd:Connector>
--         <ssd:Connector name="k2" kind="parameter">
--           <ssc:Real />
--         </ssd:Connector>
--       </ssd:Connectors>
--     </ssd:Component>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    0 warnings
-- info:    3 errors
-- endResult
