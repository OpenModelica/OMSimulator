-- status: correct
-- teardown_command: rm -rf referenceResources1_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no
-- asan: yes

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./referenceResources1_lua/")
oms_setWorkingDirectory("./referenceResources1_lua/")

oms_newModel("referenceResources1")

oms_addSystem("referenceResources1.root", oms_system_wc)
oms_addConnector("referenceResources1.root.Input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("referenceResources1.root.Input2", oms_causality_input, oms_signal_type_real)

-- add Top level resources
oms_newResources("referenceResources1.root:root.ssv")

oms_setReal("referenceResources1.root.Input1", 10)
oms_setReal("referenceResources1.root.Input2", 50)

oms_addSystem("referenceResources1.root.system1", oms_system_sc)
oms_addConnector("referenceResources1.root.system1.C1", oms_causality_input, oms_signal_type_real)
oms_addConnector("referenceResources1.root.system1.C2", oms_causality_input, oms_signal_type_real)

-- add resources to subsystem
oms_newResources("referenceResources1.root.system1:system1.ssv")
oms_setReal("referenceResources1.root.system1.C1", -10)

oms_addSubModel("referenceResources1.root.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

-- add resources to submodule
oms_newResources("referenceResources1.root.Gain:gain.ssv")

oms_setReal("referenceResources1.root.Gain.k", 27)

oms_setResultFile("referenceResources1", "referenceResources1.mat", 10)

src = oms_exportSnapshot("referenceResources1")
-- print(src)

print("info:    virgin parameter settings:")
print("info:      referenceResources1.root.Input1     : " .. oms_getReal("referenceResources1.root.Input1"))
print("info:      referenceResources1.root.Input2     : " .. oms_getReal("referenceResources1.root.Input2"))
print("info:      referenceResources1.root.system1.C1 : " .. oms_getReal("referenceResources1.root.system1.C1"))
print("info:      referenceResources1.root.system1.C2 : " .. oms_getReal("referenceResources1.root.system1.C2"))
print("info:      referenceResources1.root.Gain.k     : " .. oms_getReal("referenceResources1.root.Gain.k"))

-- add list of external resources from filesystem to ssp
oms_addResources("referenceResources1", "../../resources/externalRoot.ssv")
oms_addResources("referenceResources1", "../../resources/externalSystem1.ssv")
oms_addResources("referenceResources1", "../../resources/externalGain.ssv")

-- delete references
oms_deleteResources("referenceResources1.root:root.ssv")
-- switch with new references
oms_referenceResources("referenceResources1.root:externalRoot.ssv")

-- delete only the references
oms_deleteResources("referenceResources1.root.system1:system1.ssv")
-- switch with new references
oms_referenceResources("referenceResources1.root.system1:externalSystem1.ssv")

-- delete only the references
oms_deleteResources("referenceResources1.root.Gain:gain.ssv")
-- switch with new references
oms_referenceResources("referenceResources1.root.Gain:externalGain.ssv")

oms_instantiate("referenceResources1")

oms_initialize("referenceResources1")
print("info:    Initialization after switching to new references")
print("info:      referenceResources1.root.Input1     : " .. oms_getReal("referenceResources1.root.Input1"))
print("info:      referenceResources1.root.Input2     : " .. oms_getReal("referenceResources1.root.Input2"))
print("info:      referenceResources1.root.system1.C1 : " .. oms_getReal("referenceResources1.root.system1.C1"))
print("info:      referenceResources1.root.system1.C2 : " .. oms_getReal("referenceResources1.root.system1.C2"))
print("info:      referenceResources1.root.Gain.k     : " .. oms_getReal("referenceResources1.root.Gain.k"))

oms_simulate("referenceResources1")
print("info:    Simulation")
print("info:      referenceResources1.root.Input1     : " .. oms_getReal("referenceResources1.root.Input1"))
print("info:      referenceResources1.root.Input2     : " .. oms_getReal("referenceResources1.root.Input2"))
print("info:      referenceResources1.root.system1.C1 : " .. oms_getReal("referenceResources1.root.system1.C1"))
print("info:      referenceResources1.root.system1.C2 : " .. oms_getReal("referenceResources1.root.system1.C2"))
print("info:      referenceResources1.root.Gain.k     : " .. oms_getReal("referenceResources1.root.Gain.k"))

-- snapshot after deleting references
src = oms_exportSnapshot("referenceResources1")
print(src)


oms_terminate("referenceResources1")
oms_delete("referenceResources1")


-- Result:
-- info:    virgin parameter settings:
-- info:      referenceResources1.root.Input1     : 10.0
-- info:      referenceResources1.root.Input2     : 50.0
-- info:      referenceResources1.root.system1.C1 : -10.0
-- info:      referenceResources1.root.system1.C2 : 0.0
-- info:      referenceResources1.root.Gain.k     : 27.0
-- info:    model doesn't contain any continuous state
-- info:    Result file: referenceResources1.mat (bufferSize=10)
-- info:    Initialization after switching to new references
-- info:      referenceResources1.root.Input1     : -100.0
-- info:      referenceResources1.root.Input2     : -500.0
-- info:      referenceResources1.root.system1.C1 : -30.0
-- info:      referenceResources1.root.system1.C2 : 0.0
-- info:      referenceResources1.root.Gain.k     : -35.0
-- info:    Simulation
-- info:      referenceResources1.root.Input1     : -100.0
-- info:      referenceResources1.root.Input2     : -500.0
-- info:      referenceResources1.root.system1.C1 : -30.0
-- info:      referenceResources1.root.system1.C2 : 0.0
-- info:      referenceResources1.root.Gain.k     : -35.0
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
--       name="referenceResources1"
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
--             source="resources/externalRoot.ssv" />
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
--                 source="resources/externalSystem1.ssv" />
--             </ssd:ParameterBindings>
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
--                 <ssc:Real
--                   unit="1" />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:ParameterBindings>
--               <ssd:ParameterBinding
--                 source="resources/externalGain.ssv" />
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
--       <ssd:Units>
--         <ssc:Unit
--           name="1">
--           <ssc:BaseUnit />
--         </ssc:Unit>
--       </ssd:Units>
--       <ssd:DefaultExperiment
--         startTime="0.000000"
--         stopTime="1.000000">
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation
--                 resultFile="referenceResources1.mat"
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
--     name="resources/externalRoot.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="Input2">
--           <ssv:Real
--             value="-500" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="Input1">
--           <ssv:Real
--             value="-100" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
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
--     name="resources/externalSystem1.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="C1">
--           <ssv:Real
--             value="-30" />
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
--     name="resources/externalGain.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="k">
--           <ssv:Real
--             value="-35"
--             unit="1" />
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
--             value="27"
--             unit="1" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--       <ssv:Units>
--         <ssc:Unit
--           name="1">
--           <ssc:BaseUnit />
--         </ssc:Unit>
--       </ssv:Units>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="referenceResources1.root.Input1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="referenceResources1.root.Input2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="referenceResources1.root.Gain.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="referenceResources1.root.Gain.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="referenceResources1.root.Gain.k"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="referenceResources1.root.system1.C1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="referenceResources1.root.system1.C2"
--         type="Real"
--         kind="input" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
