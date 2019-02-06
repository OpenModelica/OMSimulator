-- status: correct
-- teardown_command: rm -rf buses-lua/
-- linux: yes
-- mingw: yes
-- win: yes
-- mac: yes

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
status = oms_addConnector("model.tlm.wc1.u1", input, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc1.u2", input, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc1.y", output, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc2.y1", output, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc2.y2", output, oms_signal_type_real)
status = oms_addConnector("model.tlm.wc2.y3", output, oms_signal_type_real)
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

src, status = oms_list("model.tlm")
print(src)

status = oms_deleteConnectorFromBus("model.tlm.wc1.bus1","model.tlm.wc1.y")
printStatus(status, 0)

src, status = oms_list("model.tlm")
print(src)

status = oms_delete("model")
printStatus(status, 0)

-- Result:
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error: [addConnectorToBus] Bus "wc1.bus1" and connector "wc2.y1" do not belong to same system
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
-- <ssd:System name="tlm">
-- 	<ssd:SimulationInformation>
-- 		<ssd:Annotations>
-- 			<ssd:Annotation type="org.openmodelica">
-- 				<oms:TlmMaster ip="" managerport="0" monitorport="0" />
-- 			</ssd:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:SimulationInformation>
-- 	<ssd:Elements>
-- 		<ssd:System name="wc2">
-- 			<ssd:SimulationInformation>
-- 				<FixedStepMaster description="oms-ma" stepSize="0.100000" />
-- 			</ssd:SimulationInformation>
-- 			<ssd:Elements />
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="y1" kind="output" type="Real" />
-- 				<ssd:Connector name="y2" kind="output" type="Real" />
-- 				<ssd:Connector name="y3" kind="output" type="Real" />
-- 			</ssd:Connectors>
-- 			<ssd:Connections />
-- 			<ssd:Annotations>
-- 				<ssd:Annotation type="org.openmodelica">
-- 					<oms:Bus name="bus2">
-- 						<oms:Signals>
-- 							<oms:Signal name="y1" />
-- 							<oms:Signal name="y2" />
-- 						</oms:Signals>
-- 					</oms:Bus>
-- 				</ssd:Annotation>
-- 			</ssd:Annotations>
-- 		</ssd:System>
-- 		<ssd:System name="wc1">
-- 			<ssd:SimulationInformation>
-- 				<FixedStepMaster description="oms-ma" stepSize="0.100000" />
-- 			</ssd:SimulationInformation>
-- 			<ssd:Elements />
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="u1" kind="input" type="Real" />
-- 				<ssd:Connector name="u2" kind="input" type="Real" />
-- 				<ssd:Connector name="y" kind="output" type="Real" />
-- 			</ssd:Connectors>
-- 			<ssd:Connections />
-- 			<ssd:Annotations>
-- 				<ssd:Annotation type="org.openmodelica">
-- 					<oms:Bus name="bus1">
-- 						<oms:Signals>
-- 							<oms:Signal name="u1" />
-- 							<oms:Signal name="u2" />
-- 							<oms:Signal name="y" />
-- 						</oms:Signals>
-- 					</oms:Bus>
-- 				</ssd:Annotation>
-- 			</ssd:Annotations>
-- 		</ssd:System>
-- 	</ssd:Elements>
-- 	<ssd:Connectors />
-- 	<ssd:Connections>
-- 		<ssd:Connection startElement="wc2" startConnector="y1" endElement="wc1" endConnector="u1" />
-- 		<ssd:Connection startElement="wc2" startConnector="y2" endElement="wc1" endConnector="u2" />
-- 	</ssd:Connections>
-- 	<ssd:Annotations>
-- 		<ssd:Annotation type="org.openmodelica">
-- 			<oms:Connections>
-- 				<oms:Connection startElement="wc1" startConnector="bus1" endElement="wc2" endConnector="bus2" />
-- 			</oms:Connections>
-- 		</ssd:Annotation>
-- 	</ssd:Annotations>
-- </ssd:System>
--
-- status:  [correct] ok
-- <?xml version="1.0"?>
-- <ssd:System name="tlm">
-- 	<ssd:SimulationInformation>
-- 		<ssd:Annotations>
-- 			<ssd:Annotation type="org.openmodelica">
-- 				<oms:TlmMaster ip="" managerport="0" monitorport="0" />
-- 			</ssd:Annotation>
-- 		</ssd:Annotations>
-- 	</ssd:SimulationInformation>
-- 	<ssd:Elements>
-- 		<ssd:System name="wc2">
-- 			<ssd:SimulationInformation>
-- 				<FixedStepMaster description="oms-ma" stepSize="0.100000" />
-- 			</ssd:SimulationInformation>
-- 			<ssd:Elements />
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="y1" kind="output" type="Real" />
-- 				<ssd:Connector name="y2" kind="output" type="Real" />
-- 				<ssd:Connector name="y3" kind="output" type="Real" />
-- 			</ssd:Connectors>
-- 			<ssd:Connections />
-- 			<ssd:Annotations>
-- 				<ssd:Annotation type="org.openmodelica">
-- 					<oms:Bus name="bus2">
-- 						<oms:Signals>
-- 							<oms:Signal name="y1" />
-- 							<oms:Signal name="y2" />
-- 						</oms:Signals>
-- 					</oms:Bus>
-- 				</ssd:Annotation>
-- 			</ssd:Annotations>
-- 		</ssd:System>
-- 		<ssd:System name="wc1">
-- 			<ssd:SimulationInformation>
-- 				<FixedStepMaster description="oms-ma" stepSize="0.100000" />
-- 			</ssd:SimulationInformation>
-- 			<ssd:Elements />
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="u1" kind="input" type="Real" />
-- 				<ssd:Connector name="u2" kind="input" type="Real" />
-- 				<ssd:Connector name="y" kind="output" type="Real" />
-- 			</ssd:Connectors>
-- 			<ssd:Connections />
-- 			<ssd:Annotations>
-- 				<ssd:Annotation type="org.openmodelica">
-- 					<oms:Bus name="bus1">
-- 						<oms:Signals>
-- 							<oms:Signal name="u1" />
-- 							<oms:Signal name="u2" />
-- 						</oms:Signals>
-- 					</oms:Bus>
-- 				</ssd:Annotation>
-- 			</ssd:Annotations>
-- 		</ssd:System>
-- 	</ssd:Elements>
-- 	<ssd:Connectors />
-- 	<ssd:Connections>
-- 		<ssd:Connection startElement="wc2" startConnector="y1" endElement="wc1" endConnector="u1" />
-- 		<ssd:Connection startElement="wc2" startConnector="y2" endElement="wc1" endConnector="u2" />
-- 	</ssd:Connections>
-- 	<ssd:Annotations>
-- 		<ssd:Annotation type="org.openmodelica">
-- 			<oms:Connections>
-- 				<oms:Connection startElement="wc1" startConnector="bus1" endElement="wc2" endConnector="bus2" />
-- 			</oms:Connections>
-- 		</ssd:Annotation>
-- 	</ssd:Annotations>
-- </ssd:System>
--
-- status:  [correct] ok
-- info:    0 warnings
-- info:    2 errors
-- endResult
