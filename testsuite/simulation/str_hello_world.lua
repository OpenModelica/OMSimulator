-- status: correct
-- teardown_command: rm -rf str_hello_world_lua/
-- linux: no
-- ucrt64: yes
-- win: yes
-- mac: no


oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./str_hello_world_lua/")

oms_newModel("model")
oms_addSystem("model.root", oms_system_wc)

oms_addSubModel('model.root.fmu', '../resources/str_hello_world.fmu')

oms_setResultFile("model", "")

oms_instantiate("model")

print("info: a: " .. oms_getString("model.root.fmu.a"))
print("info: b: " .. oms_getString("model.root.fmu.b"))
print("info: c: " .. oms_getString("model.root.fmu.c"))

oms_setString("model.root.fmu.b", "OMSimulator")
print("info: c: " .. oms_getString("model.root.fmu.c"))

oms_initialize("model")
print("info: a: " .. oms_getString("model.root.fmu.a"))
print("info: b: " .. oms_getString("model.root.fmu.b"))
print("info: c: " .. oms_getString("model.root.fmu.c"))

src = oms_exportSnapshot("model")
print(src)

oms_terminate("model")
oms_delete("model")

-- Result:
-- info: a: Hello
-- info: b: World
-- info: c: Hello World!
-- info: c: Hello OMSimulator!
-- info:    No result file will be created
-- info: a: Hello
-- info: b: OMSimulator
-- info: c: Hello OMSimulator!
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
--             name="fmu"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0001_fmu.fmu">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="a"
--                 kind="parameter">
--                 <ssc:String />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="b"
--                 kind="parameter">
--                 <ssc:String />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="c"
--                 kind="calculatedParameter">
--                 <ssc:String />
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
--     name="resources/signalFilter.xml">
--     <oms:SignalFilter
--       version="1.0">
--       <oms:Variable
--         name="model.root.fmu.a"
--         type="String"
--         kind="parameter" />
--       <oms:Variable
--         name="model.root.fmu.b"
--         type="String"
--         kind="parameter" />
--       <oms:Variable
--         name="model.root.fmu.c"
--         type="String"
--         kind="calculatedParameter" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- endResult
