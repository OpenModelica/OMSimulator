-- status: correct
-- teardown_command: rm -rf buses-lua/
-- linux: yes
-- mingw: yes
-- win: no
-- mac: no

oms_setCommandLineOption("--suppressPath=true")

function printStatus(status, expected)
  cmp = ""
  if status == expected then
    cmp = "correct"
  else
    cmp = "wrong"
  end

  if 0 == status then
    status = "ok"
  elseif 1 == status then
    status = "warning"
  elseif 3 == status then
    status = "error"
  end
  print("status:  [" .. cmp .. "] " .. status)
end

status = oms_setTempDirectory("./buses-lua/")
printStatus(status, 0)

status = oms_newModel("model")
status = oms_addSystem("model.tlm", oms_system_tlm)
status = oms_addSystem("model.tlm.wc1", oms_system_wc)
status = oms_addSystem("model.tlm.wc2", oms_system_wc)
status = oms_addConnector("model.tlm.wc1.u1", oms_causality_input, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc1.u2", oms_causality_input, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc1.y", oms_causality_output, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc2.y1", oms_causality_output, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc2.y2", oms_causality_output, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc2.y3", oms_causality_output, oms_signal_type_real)
status = oms_addBus("model.tlm.wc1.bus1")
printStatus(status, 0)

status = oms_addConnectorToBus("model.tlm.wc1.bus1","model.tlm.wc1.u1")
printStatus(status, 0)

status = oms_addConnectorToBus("model.tlm.wc1.bus1","model.tlm.wc1.u2")
printStatus(status, 0)

status = oms_addConnectorToBus("model.tlm.wc1.bus1","model.tlm.wc2.y1")
printStatus(status, 3)

status = oms_addConnectorToBus("model.tlm.wc1.bus1","model.tlm.wc1.y")
printStatus(status, 0)

status = oms_addBus("model.tlm.wc2.bus2")
printStatus(status, 0)

status = oms_addConnectorToBus("model.tlm.wc2.bus2","model.tlm.wc2.y1")
printStatus(status, 0)

status = oms_addConnectorToBus("model.tlm.wc2.bus2","model.tlm.wc2.y2")
printStatus(status, 0)

status = oms_addConnection("model.tlm.wc1.u1","model.tlm.wc2.y1")
printStatus(status, 0)

status = oms_addConnection("model.tlm.wc1.bus1","model.tlm.wc2.bus2")
printStatus(status, 0)

status = oms_addConnection("model.tlm.wc1.u2","model.tlm.wc2.y2")
printStatus(status, 0)

status = oms_addConnection("model.tlm.wc2.bus2","model.tlm.wc1.bus1")
printStatus(status, 3)

src, status = oms_exportSnapshot("model.tlm")
print(src)

status = oms_deleteConnectorFromBus("model.tlm.wc1.bus1","model.tlm.wc1.y")
printStatus(status, 0)

src, status = oms_exportSnapshot("model.tlm")
print(src)

status = oms_delete("model")
printStatus(status, 0)


-- Result:
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [addConnectorToBus] Bus "wc1.bus1" and connector "wc2.y1" do not belong to same system
-- status:  [correct] error
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [addConnection] Connection <"wc2.bus2", "wc1.bus1"> exists already in system "model.tlm"
-- status:  [correct] error
-- <?xml version="1.0"?>
-- <oms:snapshot>
--   <oms:file name="SystemStructure.ssd">
--     <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="model" version="1.0">
--       <ssd:System name="tlm">
--         <ssd:Elements>
--           <ssd:System name="wc2">
--             <ssd:Connectors>
--               <ssd:Connector name="y1" kind="output">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector name="y2" kind="output">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector name="y3" kind="output">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:Annotations>
--               <ssc:Annotation type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:Buses>
--                     <oms:Bus name="bus2">
--                       <oms:Signals>
--                         <oms:Signal name="y1" />
--                         <oms:Signal name="y2" />
--                       </oms:Signals>
--                     </oms:Bus>
--                   </oms:Buses>
--                   <oms:SimulationInformation>
--                     <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
--                   </oms:SimulationInformation>
--                 </oms:Annotations>
--               </ssc:Annotation>
--             </ssd:Annotations>
--           </ssd:System>
--           <ssd:System name="wc1">
--             <ssd:Connectors>
--               <ssd:Connector name="u1" kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector name="u2" kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector name="y" kind="output">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:Annotations>
--               <ssc:Annotation type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:Buses>
--                     <oms:Bus name="bus1">
--                       <oms:Signals>
--                         <oms:Signal name="u1" />
--                         <oms:Signal name="u2" />
--                         <oms:Signal name="y" />
--                       </oms:Signals>
--                     </oms:Bus>
--                   </oms:Buses>
--                   <oms:SimulationInformation>
--                     <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
--                   </oms:SimulationInformation>
--                 </oms:Annotations>
--               </ssc:Annotation>
--             </ssd:Annotations>
--           </ssd:System>
--         </ssd:Elements>
--         <ssd:Connections>
--           <ssd:Connection startElement="wc2" startConnector="y1" endElement="wc1" endConnector="u1" />
--           <ssd:Connection startElement="wc2" startConnector="y2" endElement="wc1" endConnector="u2" />
--         </ssd:Connections>
--         <ssd:Annotations>
--           <ssc:Annotation type="org.openmodelica">
--             <oms:Annotations>
--               <oms:Connections>
--                 <oms:Connection startElement="wc1" startConnector="bus1" endElement="wc2" endConnector="bus2" />
--               </oms:Connections>
--               <oms:SimulationInformation>
--                 <oms:TlmMaster ip="" managerport="0" monitorport="0" />
--               </oms:SimulationInformation>
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:System>
--       <ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
--         <ssd:Annotations>
--           <ssc:Annotation type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation resultFile="model_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:DefaultExperiment>
--     </ssd:SystemStructureDescription>
--   </oms:file>
-- </oms:snapshot>
--
-- status:  [correct] ok
-- <?xml version="1.0"?>
-- <oms:snapshot>
--   <oms:file name="SystemStructure.ssd">
--     <ssd:SystemStructureDescription xmlns:ssc="http://ssp-standard.org/SSP1/SystemStructureCommon" xmlns:ssd="http://ssp-standard.org/SSP1/SystemStructureDescription" xmlns:ssv="http://ssp-standard.org/SSP1/SystemStructureParameterValues" xmlns:ssm="http://ssp-standard.org/SSP1/SystemStructureParameterMapping" xmlns:ssb="http://ssp-standard.org/SSP1/SystemStructureSignalDictionary" xmlns:oms="https://raw.githubusercontent.com/OpenModelica/OMSimulator/master/schema/oms.xsd" name="model" version="1.0">
--       <ssd:System name="tlm">
--         <ssd:Elements>
--           <ssd:System name="wc2">
--             <ssd:Connectors>
--               <ssd:Connector name="y1" kind="output">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector name="y2" kind="output">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector name="y3" kind="output">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:Annotations>
--               <ssc:Annotation type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:Buses>
--                     <oms:Bus name="bus2">
--                       <oms:Signals>
--                         <oms:Signal name="y1" />
--                         <oms:Signal name="y2" />
--                       </oms:Signals>
--                     </oms:Bus>
--                   </oms:Buses>
--                   <oms:SimulationInformation>
--                     <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
--                   </oms:SimulationInformation>
--                 </oms:Annotations>
--               </ssc:Annotation>
--             </ssd:Annotations>
--           </ssd:System>
--           <ssd:System name="wc1">
--             <ssd:Connectors>
--               <ssd:Connector name="u1" kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector name="u2" kind="input">
--                 <ssc:Real />
--               </ssd:Connector>
--               <ssd:Connector name="y" kind="output">
--                 <ssc:Real />
--               </ssd:Connector>
--             </ssd:Connectors>
--             <ssd:Annotations>
--               <ssc:Annotation type="org.openmodelica">
--                 <oms:Annotations>
--                   <oms:Buses>
--                     <oms:Bus name="bus1">
--                       <oms:Signals>
--                         <oms:Signal name="u1" />
--                         <oms:Signal name="u2" />
--                       </oms:Signals>
--                     </oms:Bus>
--                   </oms:Buses>
--                   <oms:SimulationInformation>
--                     <oms:FixedStepMaster description="oms-ma" stepSize="0.100000" absoluteTolerance="0.000100" relativeTolerance="0.000100" />
--                   </oms:SimulationInformation>
--                 </oms:Annotations>
--               </ssc:Annotation>
--             </ssd:Annotations>
--           </ssd:System>
--         </ssd:Elements>
--         <ssd:Connections>
--           <ssd:Connection startElement="wc2" startConnector="y1" endElement="wc1" endConnector="u1" />
--           <ssd:Connection startElement="wc2" startConnector="y2" endElement="wc1" endConnector="u2" />
--         </ssd:Connections>
--         <ssd:Annotations>
--           <ssc:Annotation type="org.openmodelica">
--             <oms:Annotations>
--               <oms:Connections>
--                 <oms:Connection startElement="wc1" startConnector="bus1" endElement="wc2" endConnector="bus2" />
--               </oms:Connections>
--               <oms:SimulationInformation>
--                 <oms:TlmMaster ip="" managerport="0" monitorport="0" />
--               </oms:SimulationInformation>
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:System>
--       <ssd:DefaultExperiment startTime="0.000000" stopTime="1.000000">
--         <ssd:Annotations>
--           <ssc:Annotation type="org.openmodelica">
--             <oms:Annotations>
--               <oms:SimulationInformation resultFile="model_res.mat" loggingInterval="0.000000" bufferSize="10" signalFilter=".*" />
--             </oms:Annotations>
--           </ssc:Annotation>
--         </ssd:Annotations>
--       </ssd:DefaultExperiment>
--     </ssd:SystemStructureDescription>
--   </oms:file>
-- </oms:snapshot>
--
-- status:  [correct] ok
-- info:    0 warnings
-- info:    2 errors
-- endResult
