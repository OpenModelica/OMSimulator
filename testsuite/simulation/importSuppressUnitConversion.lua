-- status: correct
-- teardown_command: rm -rf importsuppressunit_lua/
-- linux: no
-- ucrt64: yes
-- win: yes
-- mac: no

oms_setCommandLineOption("--suppressPath=true")
oms_setTempDirectory("./importsuppressunit_lua/")

oms_newModel("model")

oms_addSystem("model.root", oms_system_wc)

oms_addSubModel("model.root.A", "../resources/A.fmu")
oms_addSubModel("model.root.B", "../resources/B.fmu")

-- invalid connection with different unit
oms_addConnection("model.root.A.y", "model.root.B.u")
-- valid connection mm to m
oms_addConnection("model.root.A.y1", "model.root.B.u1", true)
oms_addConnection("model.root.A.y2", "model.root.B.u2", true)

oms_setResultFile("model", "importsuppressunits.mat")

oms_export("model", "importsuppressunitConversion.ssp")

oms_terminate("model")
oms_delete("model")

oms_importFile("importsuppressunitConversion.ssp")

src, status = oms_exportSnapshot("model")
print(src)

oms_instantiate("model")
oms_initialize("model")
print("info:    Initialization")
print("info:      model.root.A.y1     : " .. oms_getReal("model.root.A.y1"))
print("info:      model.root.B.u1     : " .. oms_getReal("model.root.B.u1"))
print("info:      model.root.A.y2     : " .. oms_getReal("model.root.A.y2"))
print("info:      model.root.B.u2     : " .. oms_getReal("model.root.B.u2"))

oms_simulate("model")
print("info:    Simulation")
print("info:      model.root.A.y1     : " .. oms_getReal("model.root.A.y1"))
print("info:      model.root.B.u1     : " .. oms_getReal("model.root.B.u1"))
print("info:      model.root.A.y2     : " .. oms_getReal("model.root.A.y2"))
print("info:      model.root.B.u2     : " .. oms_getReal("model.root.B.u2"))

oms_terminate("model")
oms_delete("model")



-- Result:
-- error:   [addConnection] Unit mismatch in connection: A.y -> B.u
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
--       version="2.0">
--       <ssd:System
--         name="root">
--         <ssd:Elements>
--           <ssd:Component
--             name="B"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0002_B.fmu">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="u"
--                 kind="input">
--                 <ssc:Real
--                   unit="m/s" />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.250000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="u1"
--                 kind="input">
--                 <ssc:Real
--                   unit="m" />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="u2"
--                 kind="input">
--                 <ssc:Real
--                   unit="m/ms" />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.750000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="y"
--                 kind="output">
--                 <ssc:Real
--                   unit="m" />
--                 <ssd:ConnectorGeometry
--                   x="1.000000"
--                   y="0.500000" />
--               </ssd:Connector>
--             </ssd:Connectors>
--           </ssd:Component>
--           <ssd:Component
--             name="A"
--             type="application/x-fmu-sharedlibrary"
--             source="resources/0001_A.fmu">
--             <ssd:Connectors>
--               <ssd:Connector
--                 name="u"
--                 kind="input">
--                 <ssc:Real
--                   unit="m/s" />
--                 <ssd:ConnectorGeometry
--                   x="0.000000"
--                   y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="y"
--                 kind="output">
--                 <ssc:Real
--                   unit="m" />
--                 <ssd:ConnectorGeometry
--                   x="1.000000"
--                   y="0.250000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="y1"
--                 kind="output">
--                 <ssc:Real
--                   unit="mm" />
--                 <ssd:ConnectorGeometry
--                   x="1.000000"
--                   y="0.500000" />
--               </ssd:Connector>
--               <ssd:Connector
--                 name="y2"
--                 kind="output">
--                 <ssc:Real
--                   unit="mm/s" />
--                 <ssd:ConnectorGeometry
--                   x="1.000000"
--                   y="0.750000" />
--               </ssd:Connector>
--             </ssd:Connectors>
--           </ssd:Component>
--         </ssd:Elements>
--         <ssd:Connections>
--           <ssd:Connection
--             startElement="A"
--             startConnector="y1"
--             endElement="B"
--             endConnector="u1"
--             suppressUnitConversion="true" />
--           <ssd:Connection
--             startElement="A"
--             startConnector="y2"
--             endElement="B"
--             endConnector="u2"
--             suppressUnitConversion="true" />
--         </ssd:Connections>
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
--       <ssd:Units>
--         <ssc:Unit
--           name="m">
--           <ssc:BaseUnit
--             m="1" />
--         </ssc:Unit>
--         <ssc:Unit
--           name="m/ms">
--           <ssc:BaseUnit
--             factor="1000.0"
--             m="1"
--             s="-1" />
--         </ssc:Unit>
--         <ssc:Unit
--           name="m/s">
--           <ssc:BaseUnit
--             m="1"
--             s="-1" />
--         </ssc:Unit>
--         <ssc:Unit
--           name="mm">
--           <ssc:BaseUnit
--             factor="0.001"
--             m="1" />
--         </ssc:Unit>
--         <ssc:Unit
--           name="mm/s">
--           <ssc:BaseUnit
--             factor="0.001"
--             m="1"
--             s="-1" />
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
--                 resultFile="importsuppressunits.mat"
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
--       version="2.0">
--       <oms:Variable
--         name="model.root.B.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="model.root.B.u1"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="model.root.B.u2"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="model.root.B.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="model.root.A.x"
--         type="Real"
--         kind="unknown" />
--       <oms:Variable
--         name="model.root.A.der(x)"
--         type="Real"
--         kind="unknown" />
--       <oms:Variable
--         name="model.root.A.u"
--         type="Real"
--         kind="input" />
--       <oms:Variable
--         name="model.root.A.y"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="model.root.A.y1"
--         type="Real"
--         kind="output" />
--       <oms:Variable
--         name="model.root.A.y2"
--         type="Real"
--         kind="output" />
--     </oms:SignalFilter>
--   </oms:file>
-- </oms:snapshot>
--
-- info:    Result file: importsuppressunits.mat (bufferSize=1)
-- info:    Initialization
-- info:      model.root.A.y1     : 5.0
-- info:      model.root.B.u1     : 5.0
-- info:      model.root.A.y2     : 5.0
-- info:      model.root.B.u2     : 5.0
-- info:    Simulation
-- info:      model.root.A.y1     : 5.0
-- info:      model.root.B.u1     : 5.0
-- info:      model.root.A.y2     : 5.0
-- info:      model.root.B.u2     : 5.0
-- info:    0 warnings
-- info:    1 errors
-- endResult
