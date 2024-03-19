-- status: correct
-- teardown_command: rm -rf fmi_attributes_lua/
-- linux: no
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./fmi_attributes_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.A", "../resources/fmi_attributes_19.fmu")

oms_setResultFile("model", "fmi_attributes.mat")

oms_setStopTime("model", 10.0)
oms_setSolver("model", oms_solver_wc_mav)

oms_export("model", "fmi_attributes_19.ssp")
oms_terminate("model")
oms_delete("model")

oms_importFile("fmi_attributes_19.ssp")

src, status = oms_exportSnapshot("model")
print(src)

oms_instantiate("model")
oms_initialize("model")
print("info:    Initialization")
print("info:      model.root.A.x1      : " .. oms_getReal("model.root.A.x1"))
print("info:      model.root.A.rp1     : " .. oms_getReal("model.root.A.rp1"))
print("info:      model.root.A.rp2     : " .. oms_getInteger("model.root.A.rp2"))
print("info:      model.root.A.rp3     : " .. oms_getBoolean("model.root.A.rp3"))

oms_simulate("model")
print("info:    Simulation")
print("info:      model.root.A.x1      : " .. oms_getReal("model.root.A.x1"))
print("info:      model.root.A.rp1     : " .. oms_getReal("model.root.A.rp1"))
print("info:      model.root.A.rp2     : " .. oms_getInteger("model.root.A.rp2"))
print("info:      model.root.A.rp3     : " .. oms_getBoolean("model.root.A.rp3"))

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
--         <ssd:Elements>
--           <ssd:Component
--             name="A"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0001_A.fmu">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="rp1"
--                 kind="parameter">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="rp2"
--                 kind="parameter">
--                 <ssc:Integer />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="rp3"
--                 kind="parameter">
--                 <ssc:Boolean />
--               </ssd:Connector>
--             </ssd:Connectors>
--           </ssd:Component>
--         </ssd:Elements>
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation>
--                 <oms:VariableStepMaster
--                   description="oms-mav"
--                   initialStepSize="0.000001"
--                   minimumStepSize="0.000000"
--                   maximumStepSize="0.001000"
--                   absoluteTolerance="0.000100"
--                   relativeTolerance="0.000100" />
--               </oms:SimulationInformation>
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:System>
--       <ssd:DefaultExperiment
--         startTime="0.000000"
--         stopTime="10.000000">
--         <ssd:Annotations>
--           <ssc:Annotation
--             type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation
--                 resultFile="fmi_attributes.mat"
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
--         name="model.root.A.x"
--         type="Real"
--         kind="unknown" />
--       <oms:Variable
--         name="model.root.A.x1"
--         type="Real"
--         kind="unknown" />
--       <oms:Variable
--         name="model.root.A.rp1"
--         type="Real"
--         kind="parameter" />
--       <oms:Variable
--         name="model.root.A.x2"
--         type="Integer"
--         kind="unknown" />
--       <oms:Variable
--         name="model.root.A.rp2"
--         type="Integer"
--         kind="parameter" />
--       <oms:Variable
--         name="model.root.A.x3"
--         type="Bool"
--         kind="unknown" />
--       <oms:Variable
--         name="model.root.A.rp3"
--         type="Bool"
--         kind="parameter" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    Result file: fmi_attributes.mat (bufferSize=1)
-- info:    Initialization
-- info:      model.root.A.x1      : 3.0
-- info:      model.root.A.rp1     : 6.0
-- info:      model.root.A.rp2     : 7
-- info:      model.root.A.rp3     : 1
-- info:    Simulation
-- info:      model.root.A.x1      : 3.0
-- info:      model.root.A.rp1     : 6.0
-- info:      model.root.A.rp2     : 7
-- info:      model.root.A.rp3     : 1
-- endResult
