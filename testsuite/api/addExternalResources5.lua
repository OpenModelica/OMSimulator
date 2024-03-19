-- status: correct
-- teardown_command: rm -rf addExternalResources5_from_ssm_lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./addExternalResources5_from_ssm_lua/")
oms_setWorkingDirectory("./addExternalResources5_from_ssm_lua/")

oms_newModel("addExternalResources")

oms_addSystem("addExternalResources.root", oms_system_wc)
oms_addConnector("addExternalResources.root.Input1", oms_causality_input, oms_signal_type_real)
oms_addConnector("addExternalResources.root.Input2", oms_causality_input, oms_signal_type_real)

-- add external ssv resources
oms_addResources("addExternalResources", "../../resources/external_import_parameter_mapping.ssv")
-- add external ssm resources
oms_addResources("addExternalResources", "../../resources/external_import_parameter_mapping.ssm")

-- switch with new references, ssv and ssm file
oms_referenceResources("addExternalResources.root:external_import_parameter_mapping.ssv", "external_import_parameter_mapping.ssm") -- optional second argument for ssm f

oms_setResultFile("addExternalResources", "addExternalResources5.mat", 10)

src = oms_exportSnapshot("addExternalResources");
print(src)

oms_terminate("addExternalResources")
oms_delete("addExternalResources")

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
--       name="addExternalResources"
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
--             source="resources/external_import_parameter_mapping.ssv">
--             <ssd:ParameterMapping
--               source="resources/external_import_parameter_mapping.ssm" />
--           </ssd:ParameterBinding>
--         </ssd:ParameterBindings>
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
--                 resultFile="addExternalResources5.mat"
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
--     name="resources/external_import_parameter_mapping.ssv">
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
--     name="resources/external_import_parameter_mapping.ssm">
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
--         name="addExternalResources.root.Input1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="addExternalResources.root.Input2"
--         type="Real"
--         kind="input" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
