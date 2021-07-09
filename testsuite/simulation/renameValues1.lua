-- status: correct
-- teardown_command: rm -rf renameValues1_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./renameValues1_lua/")
oms_setWorkingDirectory("./renameValues1_lua/")

oms_newModel("renameValues1")
oms_addSystem("renameValues1.root", oms_system_wc)

oms_addResources("renameValues1.root:renameValues1.ssv")

oms_addSystem("renameValues1.root.system1", oms_system_sc)

oms_addConnector("renameValues1.root.system1.k1", oms_causality_parameter, oms_signal_type_real)
oms_setReal("renameValues1.root.system1.k1", -10)

oms_addSubModel("renameValues1.root.add_1", "../../resources/Modelica.Blocks.Math.Add.fmu")
oms_setReal("renameValues1.root.add_1.k1", 10)
oms_setReal("renameValues1.root.add_1.k2", 20)
oms_setReal("renameValues1.root.add_1.u1", 40)

oms_addSubModel("renameValues1.root.add_2", "../../resources/Modelica.Blocks.Math.Add.fmu")

oms_addConnection("renameValues1.root.add_1.y", "renameValues1.root.add_2.u1")

oms_export("renameValues1", "renamevalues1.ssp")

oms_terminate("renameValues1")
oms_delete("renameValues1")

oms_importFile("renamevalues1.ssp")

oms_rename("renameValues1.root.add_1", "add_3")
oms_rename("renameValues1.root.system1", "system2")

src = oms_exportSnapshot("renameValues1")
print(src)

oms_instantiate("renameValues1")
print("info:      Instantiation")
print("info:      renameValues1.root.system2.k1     : " .. oms_getReal("renameValues1.root.system2.k1"))
print("info:      renameValues1.root.add_3.k1       : " .. oms_getReal("renameValues1.root.add_3.k1"))
print("info:      renameValues1.root.add_3.k2       : " .. oms_getReal("renameValues1.root.add_3.k2"))
print("info:      renameValues1.root.add_3.u1       : " .. oms_getReal("renameValues1.root.add_3.u1"))

oms_initialize("renameValues1")
print("info:      Initialization")
print("info:      renameValues1.root.system2.k1     : " .. oms_getReal("renameValues1.root.system2.k1"))
print("info:      renameValues1.root.add_3.k1       : " .. oms_getReal("renameValues1.root.add_3.k1"))
print("info:      renameValues1.root.add_3.k2       : " .. oms_getReal("renameValues1.root.add_3.k2"))
print("info:      renameValues1.root.add_3.u1       : " .. oms_getReal("renameValues1.root.add_3.u1"))

oms_simulate("renameValues1")
print("info:      Simulation")
print("info:      renameValues1.root.system2.k1     : " .. oms_getReal("renameValues1.root.system2.k1"))
print("info:      renameValues1.root.add_3.k1       : " .. oms_getReal("renameValues1.root.add_3.k1"))
print("info:      renameValues1.root.add_3.k2       : " .. oms_getReal("renameValues1.root.add_3.k2"))
print("info:      renameValues1.root.add_3.u1       : " .. oms_getReal("renameValues1.root.add_3.u1"))


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
--       name="renameValues1"
--       version="1.0">
--       <ssd:System
--         name="root">
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding
--             source="resources/renameValues1.ssv" />
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
--                       initialStepSize="1e-06" />
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
--                 resultFile="renameValues1_res.mat"
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
--     name="resources/renameValues1.ssv">
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
--         name="renameValues1.root.add_3.u1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="renameValues1.root.add_3.u2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="renameValues1.root.add_3.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="renameValues1.root.add_3.k1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="renameValues1.root.add_3.k2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="renameValues1.root.add_2.u1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="renameValues1.root.add_2.u2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="renameValues1.root.add_2.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="renameValues1.root.add_2.k1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="renameValues1.root.add_2.k2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="renameValues1.root.system2.k1"
--         type="Real"
--         kind="parameter" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    model doesn't contain any continuous state
-- info:      Instantiation
-- info:      renameValues1.root.system2.k1     : -10.0
-- info:      renameValues1.root.add_3.k1       : 10.0
-- info:      renameValues1.root.add_3.k2       : 20.0
-- info:      renameValues1.root.add_3.u1       : 40.0
-- info:    Result file: renameValues1_res.mat (bufferSize=10)
-- info:      Initialization
-- info:      renameValues1.root.system2.k1     : -10.0
-- info:      renameValues1.root.add_3.k1       : 10.0
-- info:      renameValues1.root.add_3.k2       : 20.0
-- info:      renameValues1.root.add_3.u1       : 40.0
-- info:      Simulation
-- info:      renameValues1.root.system2.k1     : -10.0
-- info:      renameValues1.root.add_3.k1       : 10.0
-- info:      renameValues1.root.add_3.k2       : 20.0
-- info:      renameValues1.root.add_3.u1       : 40.0
-- endResult
