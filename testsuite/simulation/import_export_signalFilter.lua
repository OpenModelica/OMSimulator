-- status: correct
-- teardown_command: rm -rf import_export_signalFilter_lua/
-- linux: yes
-- linux32: yes
-- mingw32: yes
-- mingw64: yes
-- win: yes
-- mac: no

function readFile(filename)
  local f = assert(io.open(filename, "r"))
  local content = f:read("*all")
  print("\n")
  print(content)
  f:close()
end

oms_setCommandLineOption("--suppressPath=true --skipCSVHeader=true --addParametersToCSV=true")
oms_setTempDirectory("./import_export_signalFilter_lua/")
oms_setWorkingDirectory("./import_export_signalFilter_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_newResources("model.root:model.ssv")

oms_addSystem("model.root.System1", oms_system_sc)

oms_addConnector("model.root.System1.Input_1", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.root.System1.parameter_1", oms_causality_parameter, oms_signal_type_real)

oms_setReal("model.root.System1.Input_1", 10)
oms_setReal("model.root.System1.parameter_1", 20)

oms_addSystem("model.root.System2", oms_system_sc)

oms_addConnector("model.root.System2.Input_1", oms_causality_input, oms_signal_type_real)
oms_addConnector("model.root.System2.parameter_1", oms_causality_parameter, oms_signal_type_real)

oms_setReal("model.root.System2.Input_1", 30)
oms_setReal("model.root.System2.parameter_1", 40)

oms_addSubModel("model.root.Gain", "../../resources/Modelica.Blocks.Math.Gain.fmu")

oms_removeSignalsFromResults("model", ".*")

oms_addSignalsToResults("model", "model.root.System2.*")
oms_addSignalsToResults("model", "model.root.Gain.*")

oms_removeSignalsFromResults("model", "model.root.System2.parameter_1")
oms_removeSignalsFromResults("model", "model.root.Gain.y")
-- oms_removeSignalsFromResults("model", "model.root.Gain.k")

oms_export("model", "signalFilter.ssp")

oms_terminate("model")
oms_delete("model")

oms_importFile("signalFilter.ssp")

src, status = oms_exportSnapshot("model")
print(src)

oms_instantiate("model")

oms_setStartTime("model", 0.0)
oms_setStopTime("model", 0.0)
oms_setResultFile("model", "model_res.csv")

oms_initialize("model")
readFile("model_res.csv")

oms_terminate("model")
oms_delete("model")


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
--       name="model"
--       version="1.0">
--       <ssd:System
--         name="root">
--         <ssd:ParameterBindings>
--           <ssd:ParameterBinding
--             source="resources/model.ssv" />
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
--                 name="parameter_1"
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
--                 resultFile="model_res.mat"
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
--     name="resources/model.ssv">
--     <ssv:ParameterSet
--       xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon"
--       xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues"
--       version="1.0"
--       name="parameters">
--       <ssv:Parameters>
--         <ssv:Parameter
--           name="System2.parameter_1">
--           <ssv:Real
--             value="40" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="System2.Input_1">
--           <ssv:Real
--             value="30" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="System1.parameter_1">
--           <ssv:Real
--             value="20" />
--         </ssv:Parameter>
--         <ssv:Parameter
--           name="System1.Input_1">
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
--         name="model.root.Gain.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="model.root.Gain.k"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="model.root.System2.Input_1"
--         type="Real"
--         kind="input" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    model doesn't contain any continuous state
-- info:    model doesn't contain any continuous state
-- info:    Result file: model_res.csv (bufferSize=1)
--
--
-- time,model.root.Gain.u,model.root.System2.Input_1,model.root.Gain.k
-- 0, 0, 30, 1
--
-- endResult
