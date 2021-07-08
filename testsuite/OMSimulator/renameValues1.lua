-- status: correct
-- teardown_command: rm -rf rename_values_01_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: no
-- mac: no


oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./rename_values_01_lua/")

oms_newModel("renamevalues")
oms_addSystem("renamevalues.root", oms_system_wc)

oms_addResources("renamevalues.root:renamevalues.ssv")

oms_addSystem("renamevalues.root.system1", oms_system_sc)

oms_addConnector("renamevalues.root.system1.k1", oms_causality_parameter, oms_signal_type_real)
oms_setReal("renamevalues.root.system1.k1", -10)

oms_addSubModel("renamevalues.root.add_1", "../resources/Modelica.Blocks.Math.Add.fmu")
oms_setReal("renamevalues.root.add_1.k1", 10)
oms_setReal("renamevalues.root.add_1.k2", 20)
oms_setReal("renamevalues.root.add_1.u1", 40)

oms_addSubModel("renamevalues.root.add_2", "../resources/Modelica.Blocks.Math.Add.fmu")

oms_addConnection("renamevalues.root.add_1.y", "renamevalues.root.add_2.u1")

oms_export("renamevalues", "renamevalues1.ssp")

oms_terminate("renamevalues")
oms_delete("renamevalues")

oms_importFile("renamevalues1.ssp")

oms_rename("renamevalues.root.add_1", "add_3")
oms_rename("renamevalues.root.system1", "system2")

src = oms_exportSnapshot("renamevalues")
print(src)

oms_instantiate("renamevalues")
print("info:      Instantiation")
print("info:      renamevalues.root.system2.k1     : " .. oms_getReal("renamevalues.root.system2.k1"))
print("info:      renamevalues.root.add_3.k1       : " .. oms_getReal("renamevalues.root.add_3.k1"))
print("info:      renamevalues.root.add_3.k2       : " .. oms_getReal("renamevalues.root.add_3.k2"))
print("info:      renamevalues.root.add_3.u1       : " .. oms_getReal("renamevalues.root.add_3.u1"))

oms_initialize("renamevalues")
print("info:      Initialization")
print("info:      renamevalues.root.system2.k1     : " .. oms_getReal("renamevalues.root.system2.k1"))
print("info:      renamevalues.root.add_3.k1       : " .. oms_getReal("renamevalues.root.add_3.k1"))
print("info:      renamevalues.root.add_3.k2       : " .. oms_getReal("renamevalues.root.add_3.k2"))
print("info:      renamevalues.root.add_3.u1       : " .. oms_getReal("renamevalues.root.add_3.u1"))

oms_simulate("renamevalues")
print("info:      Simulation")
print("info:      renamevalues.root.system2.k1     : " .. oms_getReal("renamevalues.root.system2.k1"))
print("info:      renamevalues.root.add_3.k1       : " .. oms_getReal("renamevalues.root.add_3.k1"))
print("info:      renamevalues.root.add_3.k2       : " .. oms_getReal("renamevalues.root.add_3.k2"))
print("info:      renamevalues.root.add_3.u1       : " .. oms_getReal("renamevalues.root.add_3.u1"))


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
--       name="renamevalues"
--       version="1.0">
--       <ssd:System
--         name="root">
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding
--             source="resources/renamevalues.ssv" />
--         </ssd:ParameterBindings>
--         <ssd:Elements>
--           <ssd:System
--             name="system2">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="k1"
--                 kind="parameter">
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
--                       initialStepSize="1e-6" />
--                   </oms:SimulationInformation>
--                 </oms:Annotations>
--               </ssc:Annotation>
--             </ssd:Annotations>
--           </ssd:System>
--           <ssd:Component
--             name="add_3"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0001_add_1.fmu">
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
--           <ssd:Component
--             name="add_2"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0002_add_2.fmu">
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
--         <ssd:Connections>
--           <ssd:Connection
--             startElement="add_3"
--             startConnector="y"
--             endElement="add_2"
--             endConnector="u1" />
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
--                 resultFile="renamevalues_res.mat"
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
--     name="resources/renamevalues.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="system2.k1">
--           <ssv:Real
--             value="-10" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="add_3.u1">
--           <ssv:Real
--             value="40" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="add_3.k2">
--           <ssv:Real
--             value="20" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="add_3.k1">
--           <ssv:Real
--             value="10" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="renamevalues.root.add_3.u1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="renamevalues.root.add_3.u2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="renamevalues.root.add_3.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="renamevalues.root.add_3.k1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="renamevalues.root.add_3.k2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="renamevalues.root.add_2.u1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="renamevalues.root.add_2.u2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="renamevalues.root.add_2.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="renamevalues.root.add_2.k1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="renamevalues.root.add_2.k2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="renamevalues.root.system2.k1"
--         type="Real"
--         kind="parameter" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    model doesn't contain any continuous state
-- info:      Instantiation
-- info:      renamevalues.root.system2.k1     : -10.0
-- info:      renamevalues.root.add_3.k1       : 10.0
-- info:      renamevalues.root.add_3.k2       : 20.0
-- info:      renamevalues.root.add_3.u1       : 40.0
-- info:    Result file: renamevalues_res.mat (bufferSize=10)
-- info:      Initialization
-- info:      renamevalues.root.system2.k1     : -10.0
-- info:      renamevalues.root.add_3.k1       : 10.0
-- info:      renamevalues.root.add_3.k2       : 20.0
-- info:      renamevalues.root.add_3.u1       : 40.0
-- info:      Simulation
-- info:      renamevalues.root.system2.k1     : -10.0
-- info:      renamevalues.root.add_3.k1       : 10.0
-- info:      renamevalues.root.add_3.k2       : 20.0
-- info:      renamevalues.root.add_3.u1       : 40.0
-- endResult
