-- status: correct
-- teardown_command: rm -rf addResources3_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./addResources3_lua/")
oms_setWorkingDirectory("./addResources3_lua/")

oms_newModel("addResources")

oms_addSystem("addResources.root", oms_system_wc)

-- add subsystem system1
oms_addSystem("addResources.root.system1", oms_system_sc);
oms_addConnector("addResources.root.system1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("addResources.root.system1.C2", oms_causality_input, oms_signal_type_real)

-- add root resources
oms_newResources("addResources.root.system1:system1.ssv")

oms_setReal("addResources.root.system1.C1", -10.5)
oms_setReal("addResources.root.system1.C2", -20.5)

oms_addSubModel("addResources.root.system1.add", "../../resources/Modelica.Blocks.Math.Add.fmu")
oms_setReal("addResources.root.system1.add.k1", 45)

oms_addSystem("addResources.root.system2", oms_system_sc);

oms_addConnector("addResources.root.system2.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("addResources.root.system2.C2", oms_causality_input, oms_signal_type_real)
-- add root resources
oms_newResources("addResources.root.system2:system2.ssv")

oms_setReal("addResources.root.system2.C1", -20)
oms_setReal("addResources.root.system2.C2", -30)

oms_addSubModel("addResources.root.system2.add2", "../../resources/Modelica.Blocks.Math.Add.fmu")
oms_setReal("addResources.root.system2.add2.k1", 49.5)

oms_setResultFile("addResources", "addResources3.mat", 10)

src = oms_exportSnapshot("addResources")
print(src)

print("info:    Virgin Parameter settings")
print("info:      addResources.root.system1.C1      : " .. oms_getReal("addResources.root.system1.C1"))
print("info:      addResources.root.system1.C2      : " .. oms_getReal("addResources.root.system1.C2"))
print("info:      addResources.root.system1.add.k1  : " .. oms_getReal("addResources.root.system1.add.k1"))
print("info:      addResources.root.system2.C1      : " .. oms_getReal("addResources.root.system2.C1"))
print("info:      addResources.root.system2.C2      : " .. oms_getReal("addResources.root.system2.C2"))
print("info:      addResources.root.system2.add2.k1 : " .. oms_getReal("addResources.root.system2.add2.k1"))

oms_instantiate("addResources")

print("info:    Parameter settings")
print("info:      addResources.root.system1.C1      : " .. oms_getReal("addResources.root.system1.C1"))
print("info:      addResources.root.system1.C2      : " .. oms_getReal("addResources.root.system1.C2"))
print("info:      addResources.root.system1.add.k1  : " .. oms_getReal("addResources.root.system1.add.k1"))
print("info:      addResources.root.system2.C1      : " .. oms_getReal("addResources.root.system2.C1"))
print("info:      addResources.root.system2.C2      : " .. oms_getReal("addResources.root.system2.C2"))
print("info:      addResources.root.system2.add2.k1 : " .. oms_getReal("addResources.root.system2.add2.k1"))

oms_initialize("addResources")

print("info:    Initialization")
print("info:      addResources.root.system1.C1      : " .. oms_getReal("addResources.root.system1.C1"))
print("info:      addResources.root.system1.C2      : " .. oms_getReal("addResources.root.system1.C2"))
print("info:      addResources.root.system1.add.k1  : " .. oms_getReal("addResources.root.system1.add.k1"))
print("info:      addResources.root.system2.C1      : " .. oms_getReal("addResources.root.system2.C1"))
print("info:      addResources.root.system2.C2      : " .. oms_getReal("addResources.root.system2.C2"))
print("info:      addResources.root.system2.add2.k1 : " .. oms_getReal("addResources.root.system2.add2.k1"))

oms_terminate("addResources")
oms_delete("addResources")

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
--       name="addResources"
--       version="2.0">
--       <ssd:System
--         name="root">
--         <ssd:Elements>
--           <ssd:System
--             name="system2">
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
--                 source="resources/system2.ssv" />
--             </ssd:ParameterBindings>
--             <ssd:Elements>
--               <ssd:Component
--                 name="add2"
--                 type="application/x-fmu-sharedlibrary"
--                 source="resources/0002_add2.fmu">
--                 <ssd:Connectors>
--                   <ssd:Connector
--                     name="u1"
--                     kind="input">
--                     <ssc:Real />
--                     <ssd:ConnectorGeometry
--                       x="0.000000"
--                       y="0.333333" />
--                   </ssd:Connector>
--                   <ssd:Connector
--                     name="u2"
--                     kind="input">
--                     <ssc:Real />
--                     <ssd:ConnectorGeometry
--                       x="0.000000"
--                       y="0.666667" />
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
--                     name="k1"
--                     kind="parameter">
--                     <ssc:Real />
--                   </ssd:Connector>
--                   <ssd:Connector
--                     name="k2"
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
--             <ssd:Elements>
--               <ssd:Component
--                 name="add"
--                 type="application/x-fmu-sharedlibrary"
--                 source="resources/0001_add.fmu">
--                 <ssd:Connectors>
--                   <ssd:Connector
--                     name="u1"
--                     kind="input">
--                     <ssc:Real />
--                     <ssd:ConnectorGeometry
--                       x="0.000000"
--                       y="0.333333" />
--                   </ssd:Connector>
--                   <ssd:Connector
--                     name="u2"
--                     kind="input">
--                     <ssc:Real />
--                     <ssd:ConnectorGeometry
--                       x="0.000000"
--                       y="0.666667" />
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
--                     name="k1"
--                     kind="parameter">
--                     <ssc:Real />
--                   </ssd:Connector>
--                   <ssd:Connector
--                     name="k2"
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
--                 resultFile="addResources3.mat"
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
--     name="resources/system2.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="2.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="add2.k1">
--           <ssv:Real
--             value="49.5" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="C2">
--           <ssv:Real
--             value="-30" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="C1">
--           <ssv:Real
--             value="-20" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/system1.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="2.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="add.k1">
--           <ssv:Real
--             value="45" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="C2">
--           <ssv:Real
--             value="-20.5" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="C1">
--           <ssv:Real
--             value="-10.5" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="2.0">
--       <oms:Variable
--         name="addResources.root.system2.C1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.system2.C2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.system2.add2.u1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.system2.add2.u2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.system2.add2.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="addResources.root.system2.add2.k1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="addResources.root.system2.add2.k2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="addResources.root.system1.C1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.system1.C2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.system1.add.u1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.system1.add.u2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.system1.add.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="addResources.root.system1.add.k1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="addResources.root.system1.add.k2"
--         type="Real"
--         kind="parameter" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    Virgin Parameter settings
-- info:      addResources.root.system1.C1      : -10.5
-- info:      addResources.root.system1.C2      : -20.5
-- info:      addResources.root.system1.add.k1  : 45.0
-- info:      addResources.root.system2.C1      : -20.0
-- info:      addResources.root.system2.C2      : -30.0
-- info:      addResources.root.system2.add2.k1 : 49.5
-- info:    model doesn't contain any continuous state
-- info:    model doesn't contain any continuous state
-- info:    Parameter settings
-- info:      addResources.root.system1.C1      : -10.5
-- info:      addResources.root.system1.C2      : -20.5
-- info:      addResources.root.system1.add.k1  : 45.0
-- info:      addResources.root.system2.C1      : -20.0
-- info:      addResources.root.system2.C2      : -30.0
-- info:      addResources.root.system2.add2.k1 : 49.5
-- info:    Result file: addResources3.mat (bufferSize=10)
-- info:    Initialization
-- info:      addResources.root.system1.C1      : -10.5
-- info:      addResources.root.system1.C2      : -20.5
-- info:      addResources.root.system1.add.k1  : 45.0
-- info:      addResources.root.system2.C1      : -20.0
-- info:      addResources.root.system2.C2      : -30.0
-- info:      addResources.root.system2.add2.k1 : 49.5
-- endResult
