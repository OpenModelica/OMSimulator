-- status: correct
-- teardown_command: rm -rf import_hierarchical_ssv_sources_01_lua/
-- linux: yes
-- mingw32: yes
-- mingw64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./import_hierarchical_ssv_sources_01_lua/")

model = oms_importFile("../resources/import_hierarchical_ssv_sources.ssp");
oms_setResultFile(model, "")

src = oms_exportSnapshot(model)
print(src)

oms_instantiate(model)

print("info:    Instantiation")
print("info:      import_hierarchical_ssv_sources.root.System1.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_2     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_2"))
print("info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.parameter_1"))

oms_initialize(model)
print("info:    Initialization")
print("info:      import_hierarchical_ssv_sources.root.System1.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_2     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_2"))
print("info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.parameter_1"))

oms_simulate(model)
print("info:    Simulate")
print("info:      import_hierarchical_ssv_sources.root.System1.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System1.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System2.Input_2     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.Input_2"))
print("info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System2.parameter_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.Input_1     : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.Input_1"))
print("info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : " .. oms_getReal("import_hierarchical_ssv_sources.root.System3.parameter_1"))

oms_terminate(model)
oms_delete(model)


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
--       name="import_hierarchical_ssv_sources"
--       version="1.0">
--       <ssd:System
--         name="root">
--         <ssd:Elements>
--           <ssd:System
--             name="System3">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="Input_1"
--                 kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="parameter_1"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:ParameterBindings>
--               <ssd:ParameterBinding
--                 source="resources/import_hierarchical_ssv_sources_system3.ssv">
--                 <ssd:ParameterMapping
--                   source="resources/import_hierarchical_ssv_sources_system3.ssm" />
--               </ssd:ParameterBinding>
--             </ssd:ParameterBindings>
--             <ssd:Annotations>
--               <ssc:Annotation
--                 type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:SimulationInformation>
--                     <oms:VariableStepSolver
--                       description="cvode"
--                       absoluteTolerance="0.0001"
--                       relativeTolerance="0.0001"
--                       minimumStepSize="0.0001"
--                       maximumStepSize="0.1"
--                       initialStepSize="0.0001" />
--                   </oms:SimulationInformation>
--                 </oms:Annotations>
--               </ssc:Annotation>
--             </ssd:Annotations>
--           </ssd:System>
--           <ssd:System
--             name="System2">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="Input_1"
--                 kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="Input_2"
--                 kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="parameter_1"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:ParameterBindings>
--               <ssd:ParameterBinding>
--                 <ssd:ParameterValues>
--                   <ssv:ParameterSet
--                     version="1.0"
--                     name="parameters">
--                     <ssv:Parameters>
--                       <ssv:Parameter
--                         name="Input_2">
--                         <ssv:Real
--                           value="-100" />
--                       </ssv:Parameter>
--                     </ssv:Parameters>
--                   </ssv:ParameterSet>
--                 </ssd:ParameterValues>
--               </ssd:ParameterBinding>
--               <ssd:ParameterBinding
--                 source="resources/import_hierarchical_ssv_sources_system2.ssv" />
--             </ssd:ParameterBindings>
--             <ssd:Annotations>
--               <ssc:Annotation
--                 type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:SimulationInformation>
--                     <oms:VariableStepSolver
--                       description="cvode"
--                       absoluteTolerance="0.0001"
--                       relativeTolerance="0.0001"
--                       minimumStepSize="0.0001"
--                       maximumStepSize="0.1"
--                       initialStepSize="0.0001" />
--                   </oms:SimulationInformation>
--                 </oms:Annotations>
--               </ssc:Annotation>
--             </ssd:Annotations>
--           </ssd:System>
--           <ssd:System
--             name="System1">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="Input_1"
--                 kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="parameter_1"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:ParameterBindings>
--               <ssd:ParameterBinding
--                 source="resources/import_hierarchical_ssv_sources_system1.ssv" />
--             </ssd:ParameterBindings>
--             <ssd:Annotations>
--               <ssc:Annotation
--                 type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:SimulationInformation>
--                     <oms:VariableStepSolver
--                       description="cvode"
--                       absoluteTolerance="0.0001"
--                       relativeTolerance="0.0001"
--                       minimumStepSize="0.0001"
--                       maximumStepSize="0.1"
--                       initialStepSize="0.0001" />
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
--                 resultFile=""
--                 loggingInterval="0.000000"
--                 bufferSize="1"
--                 signalFilter="resources/signalFilter.xml" />
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:DefaultExperiment>
--     </ssd:SystemStructureDescription>
--   </oms:file>
--   <oms:file
--     name="resources/import_hierarchical_ssv_sources_system3.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="system3_start_values">
--           <ssv:Real
--             value="-70" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/import_hierarchical_ssv_sources_system3.ssm">
--     <ssm:ParameterMapping
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping"
--       version="1.0">
--       <ssm:MappingEntry
--         source="system3_start_values"
--         target="Input_1" />
--       <ssm:MappingEntry
--         source="system3_start_values"
--         target="parameter_1" />
--     </ssm:ParameterMapping>
--   </oms:file>
--   <oms:file
--     name="resources/import_hierarchical_ssv_sources_system2.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="parameter_1">
--           <ssv:Real
--             value="-40" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="Input_1">
--           <ssv:Real
--             value="-30" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/import_hierarchical_ssv_sources_system1.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="parameter_1">
--           <ssv:Real
--             value="-20" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="Input_1">
--           <ssv:Real
--             value="-10" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="import_hierarchical_ssv_sources.root.System3.Input_1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_hierarchical_ssv_sources.root.System3.parameter_1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_hierarchical_ssv_sources.root.System2.Input_1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_hierarchical_ssv_sources.root.System2.Input_2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_hierarchical_ssv_sources.root.System2.parameter_1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_hierarchical_ssv_sources.root.System1.Input_1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_hierarchical_ssv_sources.root.System1.parameter_1"
--         type="Real"
--         kind="parameter" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    model doesn't contain any continuous state
-- info:    model doesn't contain any continuous state
-- info:    model doesn't contain any continuous state
-- info:    Instantiation
-- info:      import_hierarchical_ssv_sources.root.System1.Input_1     : -10.0
-- info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : -20.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_1     : -30.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_2     : -100.0
-- info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : -40.0
-- info:      import_hierarchical_ssv_sources.root.System3.Input_1     : -70.0
-- info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : -70.0
-- info:    No result file will be created
-- info:    Initialization
-- info:      import_hierarchical_ssv_sources.root.System1.Input_1     : -10.0
-- info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : -20.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_1     : -30.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_2     : -100.0
-- info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : -40.0
-- info:      import_hierarchical_ssv_sources.root.System3.Input_1     : -70.0
-- info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : -70.0
-- info:    Simulate
-- info:      import_hierarchical_ssv_sources.root.System1.Input_1     : -10.0
-- info:      import_hierarchical_ssv_sources.root.System1.parameter_1 : -20.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_1     : -30.0
-- info:      import_hierarchical_ssv_sources.root.System2.Input_2     : -100.0
-- info:      import_hierarchical_ssv_sources.root.System2.parameter_1 : -40.0
-- info:      import_hierarchical_ssv_sources.root.System3.Input_1     : -70.0
-- info:      import_hierarchical_ssv_sources.root.System3.parameter_1 : -70.0
-- endResult
