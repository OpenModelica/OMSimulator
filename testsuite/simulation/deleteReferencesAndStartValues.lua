-- status: correct
-- teardown_command: rm -rf deleteResourcesReferencesAndStartValues_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./deleteResourcesReferencesAndStartValues_lua/")
oms_setWorkingDirectory("./deleteResourcesReferencesAndStartValues_lua/")

oms_newModel("deleteResources")

oms_addSystem("deleteResources.root", oms_system_wc)
oms_addConnector("deleteResources.root.Input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteResources.root.Input2", oms_causality_input, oms_signal_type_real)

-- add Top level resources
oms_newResources("deleteResources.root:root.ssv")

oms_setReal("deleteResources.root.Input1", 10)
oms_setReal("deleteResources.root.Input2", 50)

oms_addSystem("deleteResources.root.system1", oms_system_sc)
oms_addConnector("deleteResources.root.system1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("deleteResources.root.system1.C2", oms_causality_input, oms_signal_type_real)

-- add resources to subsystem
oms_newResources("deleteResources.root.system1:system1.ssv")
oms_setReal("deleteResources.root.system1.C1", -10)

oms_addSubModel("deleteResources.root.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

-- add resources to submodule
oms_newResources("deleteResources.root.Gain:gain.ssv")

oms_setReal("deleteResources.root.Gain.k", 27)

oms_setResultFile("deleteResources", "deleteResources1.mat", 10)

src = oms_exportSnapshot("deleteResources")
-- print(src)

print("info:    virgin parameter settings:")
print("info:      deleteResources.root.Input1     : " .. oms_getReal("deleteResources.root.Input1"))
print("info:      deleteResources.root.Input2     : " .. oms_getReal("deleteResources.root.Input2"))
print("info:      deleteResources.root.system1.C1 : " .. oms_getReal("deleteResources.root.system1.C1"))
print("info:      deleteResources.root.system1.C2 : " .. oms_getReal("deleteResources.root.system1.C2"))
print("info:      deleteResources.root.Gain.k     : " .. oms_getReal("deleteResources.root.Gain.k"))

-- delete only the references
oms_deleteResources("deleteResources.root:root.ssv")
-- delete only the references
oms_deleteResources("deleteResources.root.system1:system1.ssv")
-- delete only the references
oms_deleteResources("deleteResources.root.Gain:gain.ssv")

oms_instantiate("deleteResources")

oms_initialize("deleteResources")
print("info:    Initialization after deleting references and start values")
print("info:      deleteResources.root.Input1     : " .. oms_getReal("deleteResources.root.Input1"))
print("info:      deleteResources.root.Input2     : " .. oms_getReal("deleteResources.root.Input2"))
print("info:      deleteResources.root.system1.C1 : " .. oms_getReal("deleteResources.root.system1.C1"))
print("info:      deleteResources.root.system1.C2 : " .. oms_getReal("deleteResources.root.system1.C2"))
print("info:      deleteResources.root.Gain.k     : " .. oms_getReal("deleteResources.root.Gain.k"))

oms_simulate("deleteResources")
print("info:    Simulation")
print("info:      deleteResources.root.Input1     : " .. oms_getReal("deleteResources.root.Input1"))
print("info:      deleteResources.root.Input2     : " .. oms_getReal("deleteResources.root.Input2"))
print("info:      deleteResources.root.system1.C1 : " .. oms_getReal("deleteResources.root.system1.C1"))
print("info:      deleteResources.root.system1.C2 : " .. oms_getReal("deleteResources.root.system1.C2"))
print("info:      deleteResources.root.Gain.k     : " .. oms_getReal("deleteResources.root.Gain.k"))

-- snapshot after deleting references
src = oms_exportSnapshot("deleteResources")
print(src)


oms_terminate("deleteResources")
oms_delete("deleteResources")



-- Result:
-- info:    virgin parameter settings:
-- info:      deleteResources.root.Input1     : 10.0
-- info:      deleteResources.root.Input2     : 50.0
-- info:      deleteResources.root.system1.C1 : -10.0
-- info:      deleteResources.root.system1.C2 : 0.0
-- info:      deleteResources.root.Gain.k     : 27.0
-- info:    model doesn't contain any continuous state
-- info:    Result file: deleteResources1.mat (bufferSize=10)
-- info:    Initialization after deleting references and start values
-- info:      deleteResources.root.Input1     : 0.0
-- info:      deleteResources.root.Input2     : 0.0
-- info:      deleteResources.root.system1.C1 : 0.0
-- info:      deleteResources.root.system1.C2 : 0.0
-- info:      deleteResources.root.Gain.k     : 1.0
-- info:    Simulation
-- info:      deleteResources.root.Input1     : 0.0
-- info:      deleteResources.root.Input2     : 0.0
-- info:      deleteResources.root.system1.C1 : 0.0
-- info:      deleteResources.root.system1.C2 : 0.0
-- info:      deleteResources.root.Gain.k     : 1.0
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
--                       description="euler"
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
