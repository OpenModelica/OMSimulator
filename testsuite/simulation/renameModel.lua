-- status: correct
-- teardown_command: rm -rf renameModel_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./renameModel_lua/")
oms_setWorkingDirectory("./renameModel_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)
oms_addConnector("model.root.input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.root.output", oms_causality_output, oms_signal_type_real)

oms_addSystem("model.root.System1", oms_system_sc)
oms_addConnector("model.root.System1.input1", oms_causality_input, oms_signal_type_real)

oms_addSubModel("model.root.add", "../../resources/Modelica.Blocks.Math.Add.fmu")

oms_rename("model", "model_3")

src = oms_exportSnapshot("model_3")
print(src)


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
--       name="model_3"
--       version="1.0">
--       <ssd:System
--         name="root">
--         <ssd:Connectors>
--           <ssd:Connector
--             name="input1"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="output"
--             kind="output">
--             <ssc:Real />
--           </ssd:Connector>
--         </ssd:Connectors>
--         <ssd:Elements>
--           <ssd:System
--             name="System1">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="input1"
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
--             name="add"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0001_add.fmu">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="u1"
--                 kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.333333" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="u2"
--                 kind="input">
--                 <ssc:Real />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.666667" />
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
--                 name="k1"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="k2"
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
--                 resultFile="model_res.mat"
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
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="model_3.root.input1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="model_3.root.output"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="model_3.root.add.u1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="model_3.root.add.u2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="model_3.root.add.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="model_3.root.add.k1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="model_3.root.add.k2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="model_3.root.System1.input1"
--         type="Real"
--         kind="input" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
