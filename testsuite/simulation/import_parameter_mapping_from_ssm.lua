-- status: correct
-- teardown_command: rm -rf import_parameter_mapping_from_ssm_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./import_parameter_mapping_from_ssm_lua/")
oms_setWorkingDirectory("./import_parameter_mapping_from_ssm_lua/")

oms_importFile("../../resources/importParameterMapping.ssp");

src = oms_exportSnapshot("import_parameter_mapping");
print(src)

oms_instantiate("import_parameter_mapping")

print("info:      Instantiation")
print("info:      import_parameter_mapping.co_sim.Input_1              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_1"))
print("info:      import_parameter_mapping.co_sim.Input_2              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_2"))
print("info:      import_parameter_mapping.co_sim.Input_3              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_3"))
print("info:      import_parameter_mapping.co_sim.parameter_1          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_1"))
print("info:      import_parameter_mapping.co_sim.parameter_2          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System1.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_1"))
print("info:      import_parameter_mapping.co_sim.System1.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_2"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System2.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_1"))
print("info:      import_parameter_mapping.co_sim.System2.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_2"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_2"))


oms_initialize("import_parameter_mapping")
print("info:    Initialization")
print("info:      import_parameter_mapping.co_sim.Input_1              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_1"))
print("info:      import_parameter_mapping.co_sim.Input_2              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_2"))
print("info:      import_parameter_mapping.co_sim.Input_3              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_3"))
print("info:      import_parameter_mapping.co_sim.parameter_1          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_1"))
print("info:      import_parameter_mapping.co_sim.parameter_2          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System1.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_1"))
print("info:      import_parameter_mapping.co_sim.System1.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_2"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System2.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_1"))
print("info:      import_parameter_mapping.co_sim.System2.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_2"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_2"))


oms_simulate("import_parameter_mapping")
print("info:    Simulation")
print("info:      import_parameter_mapping.co_sim.Input_1              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_1"))
print("info:      import_parameter_mapping.co_sim.Input_2              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_2"))
print("info:      import_parameter_mapping.co_sim.Input_3              : " .. oms_getReal("import_parameter_mapping.co_sim.Input_3"))
print("info:      import_parameter_mapping.co_sim.parameter_1          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_1"))
print("info:      import_parameter_mapping.co_sim.parameter_2          : " .. oms_getReal("import_parameter_mapping.co_sim.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System1.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_1"))
print("info:      import_parameter_mapping.co_sim.System1.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System1.Input_2"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System1.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System1.parameter_2"))

print("info:      import_parameter_mapping.co_sim.System2.Input_1      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_1"))
print("info:      import_parameter_mapping.co_sim.System2.Input_2      : " .. oms_getReal("import_parameter_mapping.co_sim.System2.Input_2"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_1  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_1"))
print("info:      import_parameter_mapping.co_sim.System2.parameter_2  : " .. oms_getReal("import_parameter_mapping.co_sim.System2.parameter_2"))

oms_terminate("import_parameter_mapping")
oms_delete("import_parameter_mapping")

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
--       name="import_parameter_mapping"
--       version="1.0">
--       <ssd:System
--         name="co_sim">
--         <ssd:Connectors>
--           <ssd:Connector
--             name="Input_1"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="Input_2"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="Input_3"
--             kind="input">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="Output_cref"
--             kind="output">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="parameter_1"
--             kind="parameter">
--             <ssc:Real />
--           </ssd:Connector>
--           <ssd:Connector
--             name="parameter_2"
--             kind="parameter">
--             <ssc:Real />
--           </ssd:Connector>
--         </ssd:Connectors>
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding
--             source="resources/import_parameter_mapping.ssv">
--             <ssd:ParameterMapping
--               source="resources/import_parameter_mapping.ssm" />
--           </ssd:ParameterBinding>
--         </ssd:ParameterBindings>
--         <ssd:Elements>
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
--               <ssd:Connector
--                 name="parameter_2"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="output"
--                 kind="output">
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
--                 name="Input_2"
--                 kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="parameter_1"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="parameter_2"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="output"
--                 kind="output">
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
--                 resultFile="import_parameter_mapping_res.mat"
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
--     name="resources/import_parameter_mapping.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="parameter_2">
--           <ssv:Real
--             value="-40" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="cosim_parameters">
--           <ssv:Real
--             value="-30" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="cosim_input">
--           <ssv:Real
--             value="20" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="Input_3">
--           <ssv:Real
--             value="50" />
--         </ssv:Parameter>
--       </ssv:Parameters>
--     </ssv:ParameterSet>
--   </oms:file>
--   <oms:file
--     name="resources/import_parameter_mapping.ssm">
--     <ssm:ParameterMapping
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping"
--       version="1.0">
--       <ssm:MappingEntry
--         source="cosim_parameters"
--         target="parameter_1" />
--       <ssm:MappingEntry
--         source="cosim_parameters"
--         target="System1.parameter_1" />
--       <ssm:MappingEntry
--         source="cosim_parameters"
--         target="System2.parameter_1" />
--       <ssm:MappingEntry
--         source="cosim_input"
--         target="Input_1" />
--       <ssm:MappingEntry
--         source="cosim_input"
--         target="Input_2" />
--       <ssm:MappingEntry
--         source="cosim_input"
--         target="System1.Input_1" />
--       <ssm:MappingEntry
--         source="cosim_input"
--         target="System1.Input_2" />
--       <ssm:MappingEntry
--         source="cosim_input"
--         target="System2.Input_1" />
--       <ssm:MappingEntry
--         source="cosim_input"
--         target="System2.Input_2" />
--     </ssm:ParameterMapping>
--   </oms:file>
--   <oms:file
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.Input_1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.Input_2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.Input_3"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.Output_cref"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.parameter_1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.parameter_2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.System2.Input_1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.System2.Input_2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.System2.parameter_1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.System2.parameter_2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.System2.output"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.System1.Input_1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.System1.Input_2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.System1.parameter_1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.System1.parameter_2"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="import_parameter_mapping.co_sim.System1.output"
--         type="Real"
--         kind="output" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    model doesn't contain any continuous state
-- info:    model doesn't contain any continuous state
-- info:      Instantiation
-- info:      import_parameter_mapping.co_sim.Input_1              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_2              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_3              : 50.0
-- info:      import_parameter_mapping.co_sim.parameter_1          : -30.0
-- info:      import_parameter_mapping.co_sim.parameter_2          : -40.0
-- info:      import_parameter_mapping.co_sim.System1.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_2  : 0.0
-- info:      import_parameter_mapping.co_sim.System2.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_2  : 0.0
-- info:    maximum step size for 'import_parameter_mapping.co_sim.System2': 0.100000
-- info:    maximum step size for 'import_parameter_mapping.co_sim.System1': 0.100000
-- info:    Result file: import_parameter_mapping_res.mat (bufferSize=10)
-- info:    Initialization
-- info:      import_parameter_mapping.co_sim.Input_1              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_2              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_3              : 50.0
-- info:      import_parameter_mapping.co_sim.parameter_1          : -30.0
-- info:      import_parameter_mapping.co_sim.parameter_2          : -40.0
-- info:      import_parameter_mapping.co_sim.System1.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_2  : 0.0
-- info:      import_parameter_mapping.co_sim.System2.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_2  : 0.0
-- info:    Simulation
-- info:      import_parameter_mapping.co_sim.Input_1              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_2              : 20.0
-- info:      import_parameter_mapping.co_sim.Input_3              : 50.0
-- info:      import_parameter_mapping.co_sim.parameter_1          : -30.0
-- info:      import_parameter_mapping.co_sim.parameter_2          : -40.0
-- info:      import_parameter_mapping.co_sim.System1.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System1.parameter_2  : 0.0
-- info:      import_parameter_mapping.co_sim.System2.Input_1      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.Input_2      : 20.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_1  : -30.0
-- info:      import_parameter_mapping.co_sim.System2.parameter_2  : 0.0
-- info:    Final Statistics for 'import_parameter_mapping.co_sim.System2':
--          NumSteps = 11 NumRhsEvals  = 12 NumLinSolvSetups = 2
--          NumNonlinSolvIters = 11 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- info:    Final Statistics for 'import_parameter_mapping.co_sim.System1':
--          NumSteps = 11 NumRhsEvals  = 12 NumLinSolvSetups = 2
--          NumNonlinSolvIters = 11 NumNonlinSolvConvFails = 0 NumErrTestFails = 0
-- endResult
