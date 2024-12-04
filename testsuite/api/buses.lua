-- status: correct
-- teardown_command: rm -rf buses-lua/
-- linux: yes
-- ucrt64: yes
-- win: yes
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
status = oms_addSystem("model.wc", oms_system_wc)
status = oms_addSystem("model.wc.sc1", oms_system_sc)
status = oms_addSystem("model.wc.sc2", oms_system_sc)
status = oms_addConnector("model.wc.sc1.u1", oms_causality_input, oms_signal_type_real)
status = oms_addConnector("model.wc.sc1.u2", oms_causality_input, oms_signal_type_real)
status = oms_addConnector("model.wc.sc1.y", oms_causality_output, oms_signal_type_real)
status = oms_addConnector("model.wc.sc2.y1", oms_causality_output, oms_signal_type_real)
status = oms_addConnector("model.wc.sc2.y2", oms_causality_output, oms_signal_type_real)
status = oms_addConnector("model.wc.sc2.y3", oms_causality_output, oms_signal_type_real)
status = oms_addBus("model.wc.sc1.bus1")
printStatus(status, 0)

status = oms_addConnectorToBus("model.wc.sc1.bus1","model.wc.sc1.u1")
printStatus(status, 0)

status = oms_addConnectorToBus("model.wc.sc1.bus1","model.wc.sc1.u2")
printStatus(status, 0)

status = oms_addConnectorToBus("model.wc.sc1.bus1","model.wc.sc2.y1")
printStatus(status, 3)

status = oms_addConnectorToBus("model.wc.sc1.bus1","model.wc.sc1.y")
printStatus(status, 0)

status = oms_addBus("model.wc.sc2.bus2")
printStatus(status, 0)

status = oms_addConnectorToBus("model.wc.sc2.bus2","model.wc.sc2.y1")
printStatus(status, 0)

status = oms_addConnectorToBus("model.wc.sc2.bus2","model.wc.sc2.y2")
printStatus(status, 0)

status = oms_addConnection("model.wc.sc1.u1","model.wc.sc2.y1")
printStatus(status, 0)

status = oms_addConnection("model.wc.sc1.bus1","model.wc.sc2.bus2")
printStatus(status, 0)

status = oms_addConnection("model.wc.sc1.u2","model.wc.sc2.y2")
printStatus(status, 0)

status = oms_addConnection("model.wc.sc2.bus2","model.wc.sc1.bus1")
printStatus(status, 3)

src, status = oms_list("model.wc")
print(src)

status = oms_deleteConnectorFromBus("model.wc.sc1.bus1","model.wc.sc1.y")
printStatus(status, 0)

src, status = oms_list("model.wc")
print(src)

status = oms_delete("model")
printStatus(status, 0)


-- Result:
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [addConnectorToBus] Bus "sc1.bus1" and connector "sc2.y1" do not belong to same system
-- status:  [correct] error
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- status:  [correct] ok
-- error:   [addConnection] Connection <"sc2.bus2", "sc1.bus1"> exists already in system "model.wc"
-- status:  [correct] error
-- <?xml version="1.0"?>
-- <ssd:System name="wc">
-- 	<ssd:Elements>
-- 		<ssd:System name="sc2">
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="y1" kind="output">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 				<ssd:Connector name="y2" kind="output">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 				<ssd:Connector name="y3" kind="output">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 			</ssd:Connectors>
-- 			<ssd:Annotations>
-- 				<ssc:Annotation type="org.openmodelica">
-- 					<oms:Annotations>
-- 						<oms:Buses>
-- 							<oms:Bus name="bus2">
-- 								<oms:Signals>
-- 									<oms:Signal name="y1" />
-- 									<oms:Signal name="y2" />
-- 								</oms:Signals>
-- 							</oms:Bus>
-- 						</oms:Buses>
-- 						<oms:SimulationInformation>
-- 							<oms:VariableStepSolver description="cvode" relativeTolerance="0.0001" minimumStepSize="1e-12" maximumStepSize="0.001" initialStepSize="1e-06" />
-- 						</oms:SimulationInformation>
-- 					</oms:Annotations>
-- 				</ssc:Annotation>
-- 			</ssd:Annotations>
-- 		</ssd:System>
-- 		<ssd:System name="sc1">
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="u1" kind="input">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 				<ssd:Connector name="u2" kind="input">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 				<ssd:Connector name="y" kind="output">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 			</ssd:Connectors>
-- 			<ssd:Annotations>
-- 				<ssc:Annotation type="org.openmodelica">
-- 					<oms:Annotations>
-- 						<oms:Buses>
-- 							<oms:Bus name="bus1">
-- 								<oms:Signals>
-- 									<oms:Signal name="u1" />
-- 									<oms:Signal name="u2" />
-- 									<oms:Signal name="y" />
-- 								</oms:Signals>
-- 							</oms:Bus>
-- 						</oms:Buses>
-- 						<oms:SimulationInformation>
-- 							<oms:VariableStepSolver description="cvode" relativeTolerance="0.0001" minimumStepSize="1e-12" maximumStepSize="0.001" initialStepSize="1e-06" />
-- 						</oms:SimulationInformation>
-- 					</oms:Annotations>
-- 				</ssc:Annotation>
-- 			</ssd:Annotations>
-- 		</ssd:System>
-- 	</ssd:Elements>
-- 	<ssd:Connections>
-- 		<ssd:Connection startElement="sc2" startConnector="y1" endElement="sc1" endConnector="u1" />
-- 		<ssd:Connection startElement="sc2" startConnector="y2" endElement="sc1" endConnector="u2" />
-- 	</ssd:Connections>
-- 	<ssd:Annotations>
-- 		<ssc:Annotation type="org.openmodelica">
-- 			<oms:Annotations>
-- 				<oms:Connections>
-- 					<oms:Connection startElement="sc1" startConnector="bus1" endElement="sc2" endConnector="bus2" />
-- 				</oms:Connections>
-- 				<oms:SimulationInformation>
-- 					<oms:FixedStepMaster description="oms-ma" stepSize="0.001000" relativeTolerance="0.000100" />
-- 				</oms:SimulationInformation>
-- 			</oms:Annotations>
-- 		</ssc:Annotation>
-- 	</ssd:Annotations>
-- </ssd:System>
--
-- status:  [correct] ok
-- <?xml version="1.0"?>
-- <ssd:System name="wc">
-- 	<ssd:Elements>
-- 		<ssd:System name="sc2">
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="y1" kind="output">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 				<ssd:Connector name="y2" kind="output">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 				<ssd:Connector name="y3" kind="output">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 			</ssd:Connectors>
-- 			<ssd:Annotations>
-- 				<ssc:Annotation type="org.openmodelica">
-- 					<oms:Annotations>
-- 						<oms:Buses>
-- 							<oms:Bus name="bus2">
-- 								<oms:Signals>
-- 									<oms:Signal name="y1" />
-- 									<oms:Signal name="y2" />
-- 								</oms:Signals>
-- 							</oms:Bus>
-- 						</oms:Buses>
-- 						<oms:SimulationInformation>
-- 							<oms:VariableStepSolver description="cvode" relativeTolerance="0.0001" minimumStepSize="1e-12" maximumStepSize="0.001" initialStepSize="1e-06" />
-- 						</oms:SimulationInformation>
-- 					</oms:Annotations>
-- 				</ssc:Annotation>
-- 			</ssd:Annotations>
-- 		</ssd:System>
-- 		<ssd:System name="sc1">
-- 			<ssd:Connectors>
-- 				<ssd:Connector name="u1" kind="input">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 				<ssd:Connector name="u2" kind="input">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 				<ssd:Connector name="y" kind="output">
-- 					<ssc:Real />
-- 				</ssd:Connector>
-- 			</ssd:Connectors>
-- 			<ssd:Annotations>
-- 				<ssc:Annotation type="org.openmodelica">
-- 					<oms:Annotations>
-- 						<oms:Buses>
-- 							<oms:Bus name="bus1">
-- 								<oms:Signals>
-- 									<oms:Signal name="u1" />
-- 									<oms:Signal name="u2" />
-- 								</oms:Signals>
-- 							</oms:Bus>
-- 						</oms:Buses>
-- 						<oms:SimulationInformation>
-- 							<oms:VariableStepSolver description="cvode" relativeTolerance="0.0001" minimumStepSize="1e-12" maximumStepSize="0.001" initialStepSize="1e-06" />
-- 						</oms:SimulationInformation>
-- 					</oms:Annotations>
-- 				</ssc:Annotation>
-- 			</ssd:Annotations>
-- 		</ssd:System>
-- 	</ssd:Elements>
-- 	<ssd:Connections>
-- 		<ssd:Connection startElement="sc2" startConnector="y1" endElement="sc1" endConnector="u1" />
-- 		<ssd:Connection startElement="sc2" startConnector="y2" endElement="sc1" endConnector="u2" />
-- 	</ssd:Connections>
-- 	<ssd:Annotations>
-- 		<ssc:Annotation type="org.openmodelica">
-- 			<oms:Annotations>
-- 				<oms:Connections>
-- 					<oms:Connection startElement="sc1" startConnector="bus1" endElement="sc2" endConnector="bus2" />
-- 				</oms:Connections>
-- 				<oms:SimulationInformation>
-- 					<oms:FixedStepMaster description="oms-ma" stepSize="0.001000" relativeTolerance="0.000100" />
-- 				</oms:SimulationInformation>
-- 			</oms:Annotations>
-- 		</ssc:Annotation>
-- 	</ssd:Annotations>
-- </ssd:System>
--
-- status:  [correct] ok
-- info:    0 warnings
-- info:    2 errors
-- endResult
