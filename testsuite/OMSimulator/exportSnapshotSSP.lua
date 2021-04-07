-- status: correct
-- teardown_command: rm -rf exportsnapshotssp_lua/
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
oms_setTempDirectory("./exportsnapshotssp_lua/")

oms_newModel("exportSnapshotSSP")

oms_addSystem("exportSnapshotSSP.root", oms_system_wc)
oms_addConnector("exportSnapshotSSP.root.Input1", oms_causality_input, oms_signal_type_real)
oms_setReal("exportSnapshotSSP.root.Input1", 10)

oms_addSubModel("exportSnapshotSSP.root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")
oms_setReal("exportSnapshotSSP.root.Gain.k", 27)

-- snapshot = oms_exportSnapshot("exportSnapshotSSP")
-- print(snapshot)

oms_export("exportSnapshotSSP", "exportSnapshotSSP.ssp")

oms_terminate("exportSnapshotSSP")
oms_delete("exportSnapshotSSP")

oms_importFile("exportSnapshotSSP.ssp")

src, status = oms_exportSnapshot("exportSnapshotSSP")
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
--       name="exportSnapshotSSP"
--       version="1.0">
--       <ssd:System
--         name="root">
--         <ssd:Connectors>
--           <ssd:Connector
--             name="Input1"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--         </ssd:Connectors>
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding
--             source="resources/exportSnapshotSSP.ssv" />
--         </ssd:ParameterBindings>
--         <ssd:Elements>
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
--                   stepSize="0.100000"
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
--                 resultFile="exportSnapshotSSP_res.mat"
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
--     name="resources/exportSnapshotSSP.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="Input1">
--           <ssv:Real
--             value="10" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="Gain.k">
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
--         name="exportSnapshotSSP.root.Input1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="exportSnapshotSSP.root.Gain.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="exportSnapshotSSP.root.Gain.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="exportSnapshotSSP.root.Gain.k"
--         type="Real"
--         kind="parameter" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
