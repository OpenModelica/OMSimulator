-- status: correct
-- teardown_command: rm -rf import_export_signalFilter2_lua/
-- linux: no
-- mac: no
-- ucrt64: yes
-- win: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./import_export_signalFilter2_lua/")
oms_setWorkingDirectory("./import_export_signalFilter2_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.testArray", "../../resources/testArray.fmu")

oms_setStopTime("model", 5.0)

oms_setResultFile("model", "testArray_res.mat")

oms_export("model", "signalFilterVars.ssp")
oms_terminate("model")
oms_delete("model")

model = oms_importFile("signalFilterVars.ssp")

src = oms_exportSnapshot(model)
print(src)

oms_instantiate(model)
oms_initialize(model)
oms_simulate(model)

print("info:    Simulation")
print("info:      model.root.testArray.a[1]     : " .. oms_getReal("model.root.testArray.a[1]"))
print("info:      model.root.testArray.a[2]     : " .. oms_getReal("model.root.testArray.a[2]"))
print("info:      model.root.testArray.a[3]     : " .. oms_getReal("model.root.testArray.a[3]"))
print("info:      model.root.testArray.x        : " .. oms_getReal("model.root.testArray.x"))

oms_terminate("model")
oms_delete("model")

-- Result:
-- warning: invalid "SystemStructureDescription" detected in file "signalFilterVars.ssp" at line: 18 column: 27, element 'Connectors' is not allowed for content model '(Connector+)'
-- warning: "SystemStructureDescription" does not conform to the SSP standard schema
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
--         <ssd:Elements>
--           <ssd:Component
--             name="testArray"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0001_testArray.fmu">
--             <ssd:Connectors />
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
--         stopTime="5.000000">
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation
--                 resultFile="testArray_res.mat"
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
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="model.root.testArray.a[1]"
--         type="Real"
--         kind="unknown" />
--       <oms:Variable
--         name="model.root.testArray.a[2]"
--         type="Real"
--         kind="unknown" />
--       <oms:Variable
--         name="model.root.testArray.a[3]"
--         type="Real"
--         kind="unknown" />
--       <oms:Variable
--         name="model.root.testArray.x"
--         type="Real"
--         kind="unknown" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    Result file: testArray_res.mat (bufferSize=1)
-- info:    Simulation
-- info:      model.root.testArray.a[1]     : 1.0
-- info:      model.root.testArray.a[2]     : 1.0
-- info:      model.root.testArray.a[3]     : 1.0
-- info:      model.root.testArray.x        : 3.0
-- info:    2 warnings
-- info:    0 errors
-- endResult
