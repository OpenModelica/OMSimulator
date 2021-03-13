-- status: correct
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true --exportParametersInline=false")
oms_setTempDirectory("./snapshot/")

oms_newModel("snapshot")
oms_addSystem("snapshot.root", oms_system_wc)

oms_addConnector("snapshot.root.C1", oms_causality_input, oms_signal_type_real)
oms_setReal("snapshot.root.C1", -10)

oms_addSubModel("snapshot.root.add", "../resources/Modelica.Blocks.Math.Add.fmu")
oms_setReal("snapshot.root.add.u1", 10)
oms_setReal("snapshot.root.add.k1", 30)

snapshot = oms_exportSnapshot("snapshot")
--print(snapshot)
oms_setReal("snapshot.root.add.u1", 3.5)

oms_importSnapshot("snapshot", snapshot)

snapshot = oms_exportSnapshot("snapshot")
print(snapshot)

oms_delete("snapshot")

-- Result:
-- <?xml version="1.0"?>
-- <oms:snapshot
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
--       name="snapshot"
--       version="1.0">
--       <ssd:System
--         name="root">
--         <ssd:Connectors>
--           <ssd:Connector
--             name="C1"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--         </ssd:Connectors>
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding
--             source="resources/snapshot.ssv" />
--         </ssd:ParameterBindings>
--         <ssd:Elements>
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
--                 resultFile="snapshot_res.mat"
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
--     name="resources/snapshot.ssv">
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
--         <ssv:Parameter
--           name="add.u1">
--           <ssv:Real
--             value="10" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="add.k1">
--           <ssv:Real
--             value="30" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0" />
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
