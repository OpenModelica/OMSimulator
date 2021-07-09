-- status: correct
-- teardown_command: rm -rf addResources_02_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./addResources_02_lua/")

oms_newModel("addResources")

oms_addSystem("addResources.root", oms_system_wc)
oms_addConnector("addResources.root.Input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("addResources.root.Input2", oms_causality_input, oms_signal_type_real)

-- add only top level resources
oms_addResources("addResources.root:root.ssv")

oms_setReal("addResources.root.Input1", 10)

oms_addSystem("addResources.root.system1", oms_system_sc)
oms_addConnector("addResources.root.system1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("addResources.root.system1.C2", oms_causality_input, oms_signal_type_real)


oms_setReal("addResources.root.system1.C1", -10)
oms_addSubModel("addResources.root.Gain", "../resources/Modelica.Blocks.Math.Gain.fmu")

oms_setReal("addResources.root.Gain.k", 27)

oms_export("addResources", "addResources2.ssp")

oms_terminate("addResources")
oms_delete("addResources")

oms_importFile("addResources2.ssp")

oms_setReal("addResources.root.Input2", 50)

oms_setResultFile("addResources", "addResources2.mat", 10)

src = oms_exportSnapshot("addResources")
print(src)

print("info:    Virgin Parameter settings")
print("info:      addResources.root.Input1     : " .. oms_getReal("addResources.root.Input1"))
print("info:      addResources.root.Input2     : " .. oms_getReal("addResources.root.Input2"))
print("info:      addResources.root.system1.C1 : " .. oms_getReal("addResources.root.system1.C1"))
print("info:      addResources.root.system1.C2 : " .. oms_getReal("addResources.root.system1.C2"))
print("info:      addResources.root.Gain.k     : " .. oms_getReal("addResources.root.Gain.k"))

oms_instantiate("addResources")

print("info:    Parameter settings")
print("info:      addResources.root.Input1     : " .. oms_getReal("addResources.root.Input1"))
print("info:      addResources.root.Input2     : " .. oms_getReal("addResources.root.Input2"))
print("info:      addResources.root.system1.C1 : " .. oms_getReal("addResources.root.system1.C1"))
print("info:      addResources.root.system1.C2 : " .. oms_getReal("addResources.root.system1.C2"))
print("info:      addResources.root.Gain.k     : " .. oms_getReal("addResources.root.Gain.k"))

oms_initialize("addResources")
print("info:    Initialization")
print("info:      addResources.root.Input1     : " .. oms_getReal("addResources.root.Input1"))
print("info:      addResources.root.Input2     : " .. oms_getReal("addResources.root.Input2"))
print("info:      addResources.root.system1.C1 : " .. oms_getReal("addResources.root.system1.C1"))
print("info:      addResources.root.system1.C2 : " .. oms_getReal("addResources.root.system1.C2"))
print("info:      addResources.root.Gain.k     : " .. oms_getReal("addResources.root.Gain.k"))

oms_simulate("addResources")
print("info:    Simulation")
print("info:      addResources.root.Input1     : " .. oms_getReal("addResources.root.Input1"))
print("info:      addResources.root.Input2     : " .. oms_getReal("addResources.root.Input2"))
print("info:      addResources.root.system1.C1 : " .. oms_getReal("addResources.root.system1.C1"))
print("info:      addResources.root.system1.C2 : " .. oms_getReal("addResources.root.system1.C2"))
print("info:      addResources.root.Gain.k     : " .. oms_getReal("addResources.root.Gain.k"))

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
--             <ssd:Annotations>
--               <ssc:Annotation
--                 type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:SimulationInformation>
--                     <oms:VariableStepSolver
--                       description="cvode"
--                       absoluteTolerance="0.000100"
--                       relativeTolerance="0.000100"
--                       minimumStepSize="0.000100"
--                       maximumStepSize="0.100000"
--                       initialStepSize="0.000100" />
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
--                 resultFile="addResources2.mat"
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
--           name="system1.C1">
--           <ssv:Real
--             value="-10" />
--         </ssv:Parameter>
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
--         name="addResources.root.Input1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.Input2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.Gain.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addResources.root.Gain.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="addResources.root.Gain.k"
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
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    Virgin Parameter settings
-- info:      addResources.root.Input1     : 10.0
-- info:      addResources.root.Input2     : 50.0
-- info:      addResources.root.system1.C1 : -10.0
-- info:      addResources.root.system1.C2 : 0.0
-- info:      addResources.root.Gain.k     : 27.0
-- info:    model doesn't contain any continuous state
-- info:    Parameter settings
-- info:      addResources.root.Input1     : 10.0
-- info:      addResources.root.Input2     : 50.0
-- info:      addResources.root.system1.C1 : -10.0
-- info:      addResources.root.system1.C2 : 0.0
-- info:      addResources.root.Gain.k     : 27.0
-- info:    Result file: addResources2.mat (bufferSize=10)
-- info:    Initialization
-- info:      addResources.root.Input1     : 10.0
-- info:      addResources.root.Input2     : 50.0
-- info:      addResources.root.system1.C1 : -10.0
-- info:      addResources.root.system1.C2 : 0.0
-- info:      addResources.root.Gain.k     : 27.0
-- info:    Simulation
-- info:      addResources.root.Input1     : 10.0
-- info:      addResources.root.Input2     : 50.0
-- info:      addResources.root.system1.C1 : -10.0
-- info:      addResources.root.system1.C2 : 0.0
-- info:      addResources.root.Gain.k     : 27.0
-- endResult
